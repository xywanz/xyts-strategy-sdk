import pandas as pd
import sqlite3
import shutil
import datetime
import argparse


def backup_db_file(file):
    cur_time = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
    shutil.copy(file, f'{file}.{cur_time}.bak')


def create_sql_conn(db_file):
    return sqlite3.connect(db_file)


def settle_position(conn):
    pos = pd.read_sql('SELECT * FROM xyts_position', conn)
    for index, row in pos.iterrows():
        pos.at[index, 'long_open_pending'] = 0
        pos.at[index, 'short_open_pending'] = 0
        pos.at[index, 'long_close_pending'] = 0
        pos.at[index, 'short_close_pending'] = 0
        pos.at[index, 'long_yd_position'] = row['long_position']
        pos.at[index, 'short_yd_position'] = row['short_position']
        pos.at[index, 'long_realized_pnl'] = 0.0
        pos.at[index, 'short_realized_pnl'] = 0.0
        pos.at[index, 'last_trade_id'] = 0
    pos = pos[(pos.long_position != 0) | (pos.short_position != 0)]

    conn.execute('DELETE FROM xyts_position')
    pos.to_sql('xyts_position', conn, if_exists='append', index=False)
    print(pos)


def settle_logical_position(conn):
    pos = pd.read_sql('SELECT * FROM xyts_logical_position', conn)
    for index, _ in pos.iterrows():
        pos.at[index, 'buy_pending'] = 0
        pos.at[index, 'sell_pending'] = 0
        pos.at[index, 'realized_pnl'] = 0.0
        pos.at[index, 'last_trade_id'] = 0
    pos = pos[pos.position != 0]

    conn.execute('DELETE FROM xyts_logical_position')
    pos.to_sql('xyts_logical_position', conn, if_exists='append', index=False)
    print(pos)


def clear_order_and_trade(conn):
    conn.execute('DELETE FROM xyts_order')
    conn.execute('DELETE FROM xyts_trade')
    conn.execute('DELETE FROM xyts_internal_trade')


def load_contract_table(contract_csv):
    df = pd.read_csv(contract_csv)
    contract_table = { row['instr']: row for _, row in df.iterrows() }
    return contract_table


def settle_expired_contract(conn, contract_table: dict):
    today = datetime.date.today()
    pos = pd.read_sql('SELECT * FROM xyts_position', conn)
    lpos = pd.read_sql('SELECT * FROM xyts_logical_position', conn)
    trades = pd.read_sql('SELECT * FROM xyts_internal_trade', conn)

    for index, row in lpos.iterrows():
        instr = row['instr']
        if instr not in contract_table.keys():
            continue
        if str(contract_table[instr]['expire_date']) == "nan" or datetime.datetime.strptime(contract_table[instr]['expire_date'], '%Y-%m-%d').date() > today:
            continue

        price = 0.0001
        col = ['account_name', 'trade_id', 'instr', 'owner', 'order_id', 'order_sys_id', 'local_timestamp', 'exchange_timestamp', 'direction', 'position_effect', 'volume', 'price']
        ts = int(datetime.datetime.now().timestamp() * 1000000)
        trade = pd.DataFrame(columns=col, data=[[row['account_name'], ts, instr, row['owner'], 0, 0, ts, ts, 'Sell' if row['volume'] > 0 else 'Buy', 'InternalTrade', abs(row['volume']), price]])
        trade_list = [trade]
        if not trades.empty:
            trade_list.append(trades)
        trades = pd.concat(trade_list, axis=0, ignore_index=True)

        lpos.at[index, 'volume'] = 0
    lpos = lpos[lpos.volume != 0]

    for index, row in pos.iterrows():
        instr = row['instr']
        if instr not in contract_table.keys():
            continue
        if str(contract_table[instr]['expire_date']) == "nan" or datetime.datetime.strptime(contract_table[instr]['expire_date'], '%Y-%m-%d').date() > today:
            continue
        pos.at[index, 'long_position'] = 0
        pos.at[index, 'short_position'] = 0
    pos = pos[(pos.long_position != 0) | (pos.short_position != 0)]

    trades.to_sql('xyts_internal_trade', conn, if_exists='append', index=False)
    conn.execute('DELETE FROM xyts_logical_position')
    lpos.to_sql('xyts_logical_position', conn, if_exists='append', index=False)
    conn.execute('DELETE FROM xyts_position')
    pos.to_sql('xyts_position', conn, if_exists='append', index=False)



def settle(db_file, contract_file):
    contract_table = load_contract_table(contract_file)

    backup_db_file(db_file)
    conn = create_sql_conn(db_file)
    settle_position(conn)
    settle_logical_position(conn)
    clear_order_and_trade(conn)
    conn.commit()

    settle_expired_contract(conn, contract_table)
    conn.commit()



if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='daily settle')
    parser.add_argument('-d', '--db', type=str, default= '../data/xyts.db', required=False)
    parser.add_argument('-c', '--contract_file', type=str, default='../conf/contracts.csv', required=False)
    args = parser.parse_args()
    settle(args.db, args.contract_file)
