#pragma once

namespace xyu {

/**
 * @brief map查找
 * 给定map和key，如果key在map里则返回const value的地址，否则返回nullptr
 * @tparam Map
 * @tparam Map::key_type
 * @param map
 * @param key
 * @return const Map::mapped_type*
 */
template <class Map, class Key = typename Map::key_type>
const typename Map::mapped_type* GetPtr(const Map& map, const Key& key) {
  auto it = map.find(key);
  return it != map.end() ? &it->second : nullptr;
}

/**
 * @brief map查找
 * 给定map和key，如果key在map里则返回value的地址，否则返回nullptr
 * @tparam Map
 * @tparam Map::key_type
 * @param map
 * @param key
 * @return const Map::mapped_type*
 */
template <class Map, class Key = typename Map::key_type>
typename Map::mapped_type* GetPtr(Map& map, const Key& key) {
  auto it = map.find(key);
  return it != map.end() ? &it->second : nullptr;
}

/**
 * @brief map查找
 * 给定一个map和一个key，如果key在map里则返回相应的value值，否则返回default的值
 * @tparam Map
 * @tparam Map::key_type
 * @tparam Value
 * @param map
 * @param key
 * @param dflt
 * @return Map::mapped_type
 */
template <class Map, class Key = typename Map::key_type, typename Value>
typename Map::mapped_type GetDefault(const Map& map, const Key& key, Value&& dflt) {
  auto it = map.find(key);
  return it != map.end() ? it->second
                         : static_cast<typename Map::mapped_type>(std::forward<Value>(dflt));
}

/**
 * @brief map查找
 * 给定一个map和一个key，如果key在map里则返回相应的value的const引用，否则返回default的const引用
 * @tparam Map
 * @tparam Map::key_type
 * @param map
 * @param key
 * @param dflt
 * @return const Map::mapped_type&
 */
template <class Map, class Key = typename Map::key_type>
const typename Map::mapped_type& GetRefDefault(const Map& map, const Key& key,
                                               const typename Map::mapped_type& dflt) {
  auto it = map.find(key);
  return it != map.end() ? it->second : dflt;
}

/// 禁止传入右值
template <class Map, class Key = typename Map::key_type>
const typename Map::mapped_type& GetRefDefault(const Map& map, const Key& key,
                                               typename Map::mapped_type&& dflt) = delete;
template <class Map, class Key = typename Map::key_type>
const typename Map::mapped_type& GetRefDefault(const Map& map, const Key& key,
                                               const typename Map::mapped_type&& dflt) = delete;

}  // namespace xyu
