#include<string>
#include<deque>
#include<iostream>
#include<numeric>


namespace otukado {


// 有理数クラス
struct rational {
    int denominator = 1; // 分母
    int numerator = 0; // 分子
  
    rational() = default;
    rational(int x) : numerator(x){}
    rational(int x, int y) : numerator(x), denominator(y) {normalize();}

    // 約分する
    void normalize() {
        // 分母が負の数にならないようにする。
        if (denominator < 0) {
            this->numerator = -this->numerator;
            this->denominator = -this->denominator;
        }
        // 最大公約数で割る
        int g = std::gcd(abs(this->numerator), this->denominator);
        this->numerator /= g;
        this->denominator /= g;
    }

    auto operator+(rational other) {
        rational res;
        // a/b + c/d のとき、分子は a*d + c*b
        res.numerator = this->numerator * other.denominator + other.numerator * this->denominator;
        // a/b + c/d のとき、分母は b*d
        res.denominator = this->denominator  * other.denominator;
        res.normalize();
        return res;
    }

    auto operator-(rational other) {
        rational res;
        // a/b + c/d のとき、分子は a*d - c*b
        res.numerator = this->numerator * other.denominator - other.numerator * this->denominator;
        // a/b + c/d のとき、分母は b*d
        res.denominator = this->denominator * other.denominator;
        res.normalize();
        return res;
    }

    auto operator*(rational other) {
        rational res;
        // 分子も分母も掛け算
        res.numerator = this->numerator * other.numerator;
        res.denominator = this->denominator * other.denominator;
        res.normalize();
        return res;
    }

    auto operator/(rational other) {
        rational res;
        // a/b / c/d のとき、分子は a*d;
        res.numerator = this->numerator * other.denominator;
        // 分母は b*c
        res.denominator = this->denominator * other.numerator;
        res.normalize();
        return res;
    }

    float value() {
        return static_cast<float> (this->numerator) / static_cast<float> (this->denominator);
    }
};


struct token {
    std::string type; // "PLUS" || "MINUS" || "MULT" || "DIV" || "LEFT" || "RIGHT"
    rational ratio;

    token(std::string type = "", rational ratio = 0, int priority = 0) : type(type), ratio(ratio) {}
};


} // namespace otukado

// 数値の読み取り
auto read_number(std::string & line, int index) {
    otukado::rational number;
    while(index < line.size() && std::isdigit(line[index])) {
        number.numerator = number.numerator * 10 + int(line[index] - '0');
        ++index;
    }
    // 小数点以下は読み取ると同時に、分母を 10 倍する
    if(index < line.size() && line[index] == '.') {
        index += 1;
        while(index < line.size() && std::isdigit(line[index])) {
            number.numerator = number.numerator * 10 + int(line[index] - '0');
            number.denominator *= 10;
            ++index;
        }
    }
    otukado::token token ("NUMBER", number);

    return std::make_pair(token, index);
};

auto read_plus(std::string & line, int index) {
    otukado::token token("PLUS");
    return std::make_pair(token, index+1);
};

auto read_minus(std::string & line, int index) {
    otukado::token token("MINUS");
    return std::make_pair(token, index+1);
};

auto read_mult(std::string & line, int index) {
    otukado::token token("MULT");
    return std::make_pair(token, index+1);
};

auto read_div(std::string & line, int index) {
    otukado::token token("DIV");
    return std::make_pair(token, index+1);
};

auto read_left(std::string & line, int index) {
    otukado::token token("LEFT");
    return std::make_pair(token, index+1);
};

auto read_right(std::string & line, int index) {
    otukado::token token("RIGHT");
    return std::make_pair(token, index+1);
};

auto tokenize(std::string line) {
    std::deque<otukado::token> tokens;
    int index = 0;
    int depth = 0;
    while(index < line.size()) {
        otukado::token token;
        if(std::isdigit(line[index])) {
            std::tie(token, index) = read_number(line, index);
        } else if (line[index] == '+') {
            std::tie(token, index) = read_plus(line, index);
        } else if (line[index] == '-') {
            std::tie(token, index) = read_minus(line, index);
        } else if (line[index] == '*') {
            std::tie(token, index) = read_mult(line, index);
        } else if (line[index] == '/') {
            std::tie(token, index) = read_div(line, index);
        } else if (line[index] == '(') {
            std::tie(token, index) = read_left(line, index);
        } else if (line[index] == ')') {
            std::tie(token, index) = read_right(line, index);
        } else {
            throw std::runtime_error("Invalid character found: " + line[index]);
        }
        tokens.push_back(token);
    }
    return tokens;
}


// 項に区切る関数
std::deque<std::deque<otukado::token>> divide_poly(std::deque<otukado::token> tokens) {
    std::deque<std::deque<otukado::token>> terms = { { otukado::token("PLUS") } };
    std::deque<otukado::token> term;
    
    int index = 0;
    int parentheses = 0; // かっこの深さ

    while(index < tokens.size()){
        if(tokens[index].type == "PLUS" || tokens[index].type == "MINUS") {
            // 加算減算で区切る
            if(parentheses == 0) {  // かっこの中ではないかつ、+ または - のとき
                terms.push_back(term);
                terms.push_back({tokens[index]}); // + と - も terms に突っ込む
                term.clear();
            } else {
                term.push_back(tokens[index]); // かっこ内なら区切らない
            }
        } else if (tokens[index].type == "LEFT") {
            parentheses++;
            term.push_back(tokens[index]);
        } else if (tokens[index].type == "RIGHT") {
            parentheses--;
            term.push_back(tokens[index]);
        } else {  // "NUMBER"
            term.push_back(tokens[index]);
        }
        index++;
    }
    // 最後の項の計算
    if(!term.empty()) terms.push_back(term);
    return terms;
}

// 項に区切る関数
std::deque<std::deque<otukado::token>> divide_term(std::deque<otukado::token> tokens) {
    std::deque<std::deque<otukado::token>> factors = { { otukado::token("MULT") } };
    std::deque<otukado::token> factor;
    
    int index = 0;
    int parentheses = 0; // かっこの深さ

    while(index < tokens.size()){
        if(tokens[index].type == "MULT" || tokens[index].type == "DIV") {
            if(parentheses == 0) { 
                factors.push_back(factor);
                factors.push_back({tokens[index]});
                factor.clear();
            } else {
                factor.push_back(tokens[index]); // かっこ内なら区切らない
            }
        } else if (tokens[index].type == "LEFT") {
            parentheses++;
            factor.push_back(tokens[index]);
        } else if (tokens[index].type == "RIGHT") {
            parentheses--;
            factor.push_back(tokens[index]);
        } else {  // "NUMBER"
            factor.push_back(tokens[index]);
        }
        index++;
    }
    // 最後の項の計算
    if(!factor.empty()) factors.push_back(factor);
    return factors;
}

otukado::token calculate_term(std::deque<otukado::token> tokens);
otukado::token calculate_factor(std::deque<otukado::token> tokens);

otukado::token calculate_poly(std::deque<otukado::token> tokens) {
    auto terms = divide_poly(tokens);
    otukado::rational ans;

    for(int index = 0; index < terms.size() - 1; ++index) {
        if(terms[index][0].type == "PLUS") {
            ans = ans + calculate_term(terms[index + 1]).ratio;
        } else if (terms[index][0].type == "MINUS") {
            ans = ans - calculate_term(terms[index + 1]).ratio;
        }
    }

    return otukado::token("NUMBER", ans);
};

otukado::token calculate_term(std::deque<otukado::token> tokens){
    auto factors = divide_term(tokens);

    otukado::rational ans(1);
    for(int index = 0; index < factors.size() - 1; ++index) {
        if(factors[index][0].type == "MULT") {
            ans = ans * calculate_factor(factors[index + 1]).ratio;
        } else if (factors[index][0].type == "DIV") {
            ans = ans / calculate_factor(factors[index + 1]).ratio;
        }
    }
    return otukado::token("NUMBER", ans);
}

auto remove_parenthesis (std::deque<otukado::token> tokens) {
    tokens.pop_back();
    tokens.pop_front();
    return tokens;
}

otukado::token calculate_factor(std::deque<otukado::token> tokens) {
    if(tokens.size() == 1) return tokens[0];
    return calculate_poly(remove_parenthesis(tokens));
}


// 評価する
otukado::rational evaluate(std::string line) {
    auto tokens = tokenize(line);
    return calculate_poly(tokens).ratio;
}

void test(std::string line, float expected_answer) {
    auto actual_answer = evaluate(line);
    if(abs(actual_answer.value() - expected_answer) < 1e-8) {
        std::cout << "PASS! " << line << " = " << expected_answer << '\n';
    } else {
        std::cout << "FAIL! " << line << " should be " << expected_answer << " but was " << actual_answer.value() << '\n';
    }
}

void run_test() {
    test("1+2", 3.0);
    test("1*2", 2);
    test("1.5+2.2", 3.7);
    test("6/2", 3);
    test("2*3+1", 7);
    test("1+3/2", 2.5);
    test("2*3+9/3", 9);
    test("0+0+0*1", 0);
    test("3-1", 2);
    test("4*3-2/1", 10);
    test("1-3", -2);
    test("(1+2)", 3);
    test("(((1+2)))", 3);
    test("2+(3*4)", 14);
    test("2+2*(3+4)", 16);
    test("(3+4)*2", 14);
    test("2+(2+4*5)*(3+4)", 156);
};


int main() {
    run_test();
}
