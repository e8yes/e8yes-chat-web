TEMPLATE = subdirs
SUBDIRS = \
    demoweb/demowebservice.pro \
    demoweb/demowebmain.pro \
    demoweb_test/util/trie_map_test \
    demoweb_test/util/lru_hash_map_test \
    demoweb_test/sql/orm/query_completion_test

CONFIG += ordered
