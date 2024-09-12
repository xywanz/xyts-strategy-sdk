import pandas as pd
import sqlite3
import datetime
import argparse



def create_sql_conn(db_file):
    return sqlite3.connect(db_file)


def create_tables_if_not_exist(conn):
    sql = """ CREATE TABLE IF NOT EXISTS historical_pnl(
                                         strategy        CHAR(100) NOT NULL,
                                         date            CHAR(100) NOT NULL,
                                         realized_pnl    REAL,
                                         PRIMARY KEY     (strategy, date));
          """
    conn.execute(sql)


def get_strategy_pnls(historical_db_conn, date):
    df = pd.read_sql(
        f'SELECT * FROM historical_pnl where date != "{date}" order by date', historical_db_conn)
    strategy_pnl = {}
    for _, row in df.iterrows():
        strategy_pnl[row['strategy']] = row['realized_pnl']
    return strategy_pnl


def update_historical_pnl(xyts_db_conn, historical_db_conn, date):
    strategy_pnl = get_strategy_pnls(historical_db_conn)
    pos = pd.read_sql('SELECT * FROM its_logical_position', xyts_db_conn)
    for _, row in pos.iterrows():
        strategy = row['owner']
        if strategy not in strategy_pnl:
            strategy_pnl[strategy] = 0
        strategy_pnl[strategy] += row['realized_pnl']
    historical_db_conn.execute(f'DELETE FROM historical_pnl where date = "{date}"')
    for strategy, pnl in strategy_pnl.items():
        historical_db_conn.execute(f'INSERT INTO historical_pnl VALUES("{strategy}", "{date}", {pnl})')


def update_historical_db(args):
    xyts_db_conn = create_sql_conn(args.xyts_db)
    historical_db_conn = create_sql_conn(args.historical_db)
    create_tables_if_not_exist(historical_db_conn)
    update_historical_pnl(xyts_db_conn, historical_db_conn, args.date)
    historical_db_conn.commit()



if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='update historical db')
    parser.add_argument('--xyts_db', type=str, default= '../data/xyts.db', required=False)
    parser.add_argument('--historical_db', type=str, default='../data/historical.db', required=False)
    parser.add_argument('--date', type=str, default=datetime.date.today().isoformat(), required=False)
    args = parser.parse_args()
    update_historical_db(args)
