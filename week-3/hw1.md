# 電卓の仕様
## 概要
有理数を計算する電卓。
<!-- 
### evaluate
1. `divide` で加算減算で分ける
2. 分けたやつの `size` が 1 だったら、`calculate_*/` を呼んで、乗算除算する
3. `calculate_+-` 加算減算する -->

### divide_polynomial
`tokens` を 項ごとにわける
(足し算引き算でわける)

### divide_term
各項を、因子にわける
（掛け算割り算でわける）

### calculate_seki
各項を計算する
- 積だけであれば、計算可能
- 積だけじゃなかったら、`divide_tokens` を呼ぶ

### calculate_wa

poly = term + term + ...
term = factor * factor * ...
factor = (poly) | number | abs(poly)

## flow
calculate_polynomial(tokens):
    terms = divide_polynomial(tokens);
    ans = 0;
    for(term : terms) {
        ans += calculate_term(term);
    }
    return ans;

calculate_term(tokens){
    factors = divide_term(tokens);
    ans = 1;
    for(factor : factors) {
        ans *= calculate_factor(factor);
    }
    return ans;
}

calculate_factor(tokens) {
    if(tokens.size() == 1) return tokens[0] (number)
    else {
        return calculate_poly(remove_parenthesis(tokens)) <- これはかっこを外さないといけない
    }
}

remove_parenthesis (tokens) {
    tokens.pop_back();
    tokens.pop_front();
    return tokens;
}

1 + (3 + 4) * 5; <- poly 足し算系
|
V divide_poly
1, (3 + 4) * 5 <- term 掛け算系
|
V divide_term
(3 + 4), 5 <- factor 足し算系

calculate_

