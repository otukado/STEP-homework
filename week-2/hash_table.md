# design document
Hash Table

## objective
挿入、検索、削除をそれぞれ $O(1)$ でできるデータ構造


## flow
### put
`key` と `value` を指定して `hash_table` に新規に追加することができる。
すでに存在する `key` が指定された場合は、新たな `value` で上書きする。
### get
`key` を指定すると `value` を取得できる。
存在しない `key` の場合は `nullopt` を返す
### erase
`key` を指定し、その要素を削除する。
削除できた場合は `true` が返される。
存在しない `key` の場合は `false` が返される。
### calculate hash
今後考える。
文字の位置と種類でいい感じにする。
### resize
要素数と `bucket_size` の大きさで `resize` するか決める。
`bucket_size` は素数の中から決定される。
今後考える。

## test
### functional test
### performance_test