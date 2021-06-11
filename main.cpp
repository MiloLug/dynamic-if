// Online C++ compiler to run C++ program online
#include <iostream>
#include <functional>
#include <vector>
#include <tuple>
#include <memory>
#include <initializer_list>
#include <string>

using namespace std;

typedef function<void()> void_fn;

class Condition {
public:
    Condition() {}
    virtual shared_ptr<Condition> share() { return shared_ptr<Condition>(new Condition(*this)); }
    
    virtual Condition &then(const void_fn &fn) { return *this; }
    virtual Condition &other(const void_fn &fn) { return *this; }

    virtual ~Condition() {}
};

class If: public Condition {
    function<bool()> m_expr;
public:
    If(bool state): m_expr([state]{return state;}), Condition() {}
    If(const function<bool()> &expr): m_expr(expr), Condition() {}
    If(const If &o): m_expr(o.m_expr) {}
    If &operator=(const If &o) {
        m_expr = o.m_expr;
        return *this;
    }
    virtual shared_ptr<Condition> share() { return shared_ptr<Condition>(new If(*this)); }
    
    virtual If &then(const void_fn &fn){
        if(!m_expr()) return *this;
        fn();
        return *this;
    }
    
    virtual If &other(const void_fn &fn){
        if(m_expr()) return *this;
        fn();
        return *this;
    }
    
    If otherIf(bool state){
        return If([expr = this->m_expr, state]{ return !expr() && state; });
    }
    If otherIf(const If &o){
        return !*this & o;
    }
    
    If operator!(){ return If([expr = this->m_expr]{ return !expr(); }); }
    If operator&(If a){ return If([expr = this->m_expr, a]{ return expr() && a.m_expr(); }); }
    If operator|(If a){ return If([expr = this->m_expr, a]{ return expr() || a.m_expr(); }); }
    operator bool(){ return m_expr(); }
    If freeze(){ return If(m_expr()); }
    
    virtual ~If() {}
};

class For: public Condition {
    void_fn m_init;
    If m_condition;
    void_fn m_postExpr;
    bool m_stateBreak = false;
public:
    For(const void_fn &init, const If &condition, const void_fn &postExpr):
        m_init(init), m_condition(condition), m_postExpr(postExpr), Condition() {}
    For(const void_fn &init, const If &condition):
        m_init(init), m_condition(condition), m_postExpr([]{}), Condition() {}
        
    For(const For &o):
        m_init(o.m_init), m_condition(o.m_condition), m_postExpr(o.m_postExpr), Condition() {}
    For &operator=(const For &o) {
        m_init = o.m_init;
        m_condition = o.m_condition;
        m_postExpr = o.m_postExpr;
        return *this;
    }
    virtual shared_ptr<Condition> share() { return shared_ptr<Condition>(new For(*this)); }
    
    virtual For &then(const void_fn &fn){
        m_stateBreak = false;
        m_init();
        while((bool)m_condition && !m_stateBreak){
            fn();
            m_postExpr();
        }
        return *this;
    }
    virtual For &other(const void_fn &fn){ return *this; }
    
    For &stop(){ m_stateBreak = true; return *this; }
    
    virtual ~For() {}
};

class ConditionExecutor {
    shared_ptr<Condition> m_condition;
    void_fn m_thenFn, m_otherFn;
public:
    ConditionExecutor(shared_ptr<Condition> condition, const void_fn &thenFn, const void_fn &otherFn):
        m_condition(condition), m_thenFn(thenFn), m_otherFn(otherFn) {}
    ConditionExecutor(shared_ptr<Condition> condition, const void_fn &thenFn):
        m_condition(condition), m_thenFn(thenFn), m_otherFn([]{}) {}
    ConditionExecutor(Condition* condition, const void_fn &thenFn, const void_fn &otherFn):
        m_condition(condition), m_thenFn(thenFn), m_otherFn(otherFn) {}
    ConditionExecutor(Condition* condition, const void_fn &thenFn):
        m_condition(condition), m_thenFn(thenFn), m_otherFn([]{}) {}
    ConditionExecutor(const ConditionExecutor &o):
        m_condition(o.m_condition), m_thenFn(o.m_thenFn), m_otherFn(o.m_otherFn) {}
    ConditionExecutor &operator=(const ConditionExecutor &o) {
        m_condition = o.m_condition;
        m_thenFn = o.m_thenFn;
        m_otherFn = o.m_otherFn;
        return *this;
    }

    ConditionExecutor &exec(){
        m_condition->then(m_thenFn);
        return *this;
    }
};

class Conditions {
    typedef vector<ConditionExecutor> conditions_t;
    vector<ConditionExecutor> m_conditions;
public:
    Conditions(const vector<ConditionExecutor> &conditions): m_conditions(conditions) {}
    Conditions(const Conditions &o): m_conditions(o.m_conditions) {}
    Conditions &operator=(const Conditions &o) {
        m_conditions = o.m_conditions;
        return *this;
    }
    Conditions &exec(){
        for(ConditionExecutor &c: m_conditions)
            c.exec();
        return *this;
    }
    
    Conditions &operator()(){ return exec(); }
};


class Number {
    double m_data;
public:
    Number(double data = 0): m_data(data) {}
    Number(const Number &o): m_data(o.m_data) {}
    Number(const string &str): m_data(stod(str)) {}
    Number(const char *str): m_data(stod(str)) {}
    
    Number &operator=(const Number &o) { m_data = o.m_data; return *this;}
    Number &operator=(const string &str) { m_data = stod(str); return *this;}
    Number &operator=(const char *str) { m_data = stod(str); return *this;}
    
    /* Number & Number */
    Number operator+(const Number &o) { return m_data + o.m_data;}
    Number operator-(const Number &o) { return m_data - o.m_data;}
    
    Number operator/(const Number &o) { return m_data / o.m_data;}
    Number operator*(const Number &o) { return m_data * o.m_data;}
    Number operator%(const Number &o) { return (int)m_data % (int)o.m_data;}
    
    bool operator<(const Number &o) { return m_data < o.m_data; }
    bool operator>(const Number &o) { return m_data > o.m_data; }
    bool operator==(const Number &o) { return m_data == o.m_data; }
    bool operator!=(const Number &o) { return m_data != o.m_data; }
    
    bool operator&&(const Number &o) { return m_data && o.m_data; }
    bool operator||(const Number &o) { return m_data || o.m_data; }
    
    Number operator|(const Number &o) { return (int)m_data | (int)o.m_data; }
    Number operator&(const Number &o) { return (int)m_data & (int)o.m_data; }
    Number operator^(const Number &o) { return (int)m_data ^ (int)o.m_data; }
    Number operator<<(const Number &o) { return (int)m_data << (int)o.m_data; }
    Number operator>>(const Number &o) { return (int)m_data >> (int)o.m_data; }
    
    Number &operator+=(const Number &o) { m_data += o.m_data; return *this; }
    Number &operator-=(const Number &o) { m_data -= o.m_data; return *this; }
    Number &operator/=(const Number &o) { m_data /= o.m_data; return *this; }
    Number &operator*=(const Number &o) { m_data *= o.m_data; return *this; }
    Number &operator%=(const Number &o) { m_data = (int)m_data % (int)o.m_data; return *this; }
    Number &operator|=(const Number &o) { m_data = (int)m_data | (int)o.m_data; return *this; }
    Number &operator&=(const Number &o) { m_data = (int)m_data & (int)o.m_data; return *this; }
    Number &operator^=(const Number &o) { m_data = (int)m_data ^ (int)o.m_data; return *this; }
    Number &operator<<=(const Number &o) { m_data = (int)m_data << (int)o.m_data; return *this; }
    Number &operator>>=(const Number &o) { m_data = (int)m_data >> (int)o.m_data; return *this; }
    
    /* Number & double */
    Number operator+(const double &n) { return m_data + n;}
    Number operator-(const double &n) { return m_data - n;}
    
    Number operator/(const double &n) { return m_data / n;}
    Number operator*(const double &n) { return m_data * n;}
    Number operator%(const double &n) { return (int)m_data % (int)n;}
    
    bool operator<(const double &n) { return m_data < n; }
    bool operator>(const double &n) { return m_data > n; }
    bool operator==(const double &n) { return m_data == n; }
    bool operator!=(const double &n) { return m_data != n; }
    
    bool operator&&(const double &n) { return m_data && n; }
    bool operator||(const double &n) { return m_data || n; }
    
    Number operator|(const double &n) { return (int)m_data | (int)n; }
    Number operator&(const double &n) { return (int)m_data & (int)n; }
    Number operator^(const double &n) { return (int)m_data ^ (int)n; }
    Number operator<<(const double &n) { return (int)m_data << (int)n; }
    Number operator>>(const double &n) { return (int)m_data >> (int)n; }
    
    Number &operator+=(const double &n) { m_data += n; return *this; }
    Number &operator-=(const double &n) { m_data -= n; return *this; }
    Number &operator/=(const double &n) { m_data /= n; return *this; }
    Number &operator*=(const double &n) { m_data *= n; return *this; }
    Number &operator%=(const double &n) { m_data = (int)m_data % (int)n; return *this; }
    Number &operator|=(const double &n) { m_data = (int)m_data | (int)n; return *this; }
    Number &operator&=(const double &n) { m_data = (int)m_data & (int)n; return *this; }
    Number &operator^=(const double &n) { m_data = (int)m_data ^ (int)n; return *this; }
    Number &operator<<=(const double &n) { m_data = (int)m_data << (int)n; return *this; }
    Number &operator>>=(const double &n) { m_data = (int)m_data >> (int)n; return *this; }
    
    /* Number & int */
    Number operator+(const int &n) { return m_data + n;}
    Number operator-(const int &n) { return m_data - n;}
    
    Number operator/(const int &n) { return m_data / n;}
    Number operator*(const int &n) { return m_data * n;}
    Number operator%(const int &n) { return (int)m_data % n;}
    
    bool operator<(const int &n) { return m_data < n; }
    bool operator>(const int &n) { return m_data > n; }
    bool operator==(const int &n) { return m_data == n; }
    bool operator!=(const int &n) { return m_data != n; }
    
    bool operator&&(const int &n) { return m_data && n; }
    bool operator||(const int &n) { return m_data || n; }
    
    Number operator|(const int &n) { return (int)m_data | n; }
    Number operator&(const int &n) { return (int)m_data & n; }
    Number operator^(const int &n) { return (int)m_data ^ n; }
    Number operator<<(const int &n) { return (int)m_data << n; }
    Number operator>>(const int &n) { return (int)m_data >> n; }
    
    Number &operator+=(const int &n) { m_data += n; return *this; }
    Number &operator-=(const int &n) { m_data -= n; return *this; }
    Number &operator/=(const int &n) { m_data /= n; return *this; }
    Number &operator*=(const int &n) { m_data *= n; return *this; }
    Number &operator%=(const int &n) { m_data = (int)m_data % n; return *this; }
    Number &operator|=(const int &n) { m_data = (int)m_data | n; return *this; }
    Number &operator&=(const int &n) { m_data = (int)m_data & n; return *this; }
    Number &operator^=(const int &n) { m_data = (int)m_data ^ n; return *this; }
    Number &operator<<=(const int &n) { m_data = (int)m_data << n; return *this; }
    Number &operator>>=(const int &n) { m_data = (int)m_data >> n; return *this; }
    
    /* Number & string */
    Number operator+(const string &n) { return m_data + stod(n); }
    Number operator-(const string &n) { return m_data - stod(n); }
    
    Number operator/(const string &n) { return m_data / stod(n); }
    Number operator*(const string &n) { return m_data * stod(n); }
    Number operator%(const string &n) { return (int)m_data % stoi(n); }
    
    bool operator<(const string &n) { return m_data < stod(n); }
    bool operator>(const string &n) { return m_data > stod(n); }
    bool operator==(const string &n) { return m_data == stod(n); }
    bool operator!=(const string &n) { return m_data != stod(n); }
    
    bool operator&&(const string &n) { return m_data && stod(n); }
    bool operator||(const string &n) { return m_data || stod(n); }
    
    Number operator|(const string &n) { return (int)m_data | stoi(n); }
    Number operator&(const string &n) { return (int)m_data & stoi(n); }
    Number operator^(const string &n) { return (int)m_data ^ stoi(n); }
    Number operator<<(const string &n) { return (int)m_data << stoi(n); }
    Number operator>>(const string &n) { return (int)m_data >> stoi(n); }
    
    Number &operator+=(const string &n) { m_data += stod(n); return *this; }
    Number &operator-=(const string &n) { m_data -= stod(n); return *this; }
    Number &operator/=(const string &n) { m_data /= stod(n); return *this; }
    Number &operator*=(const string &n) { m_data *= stod(n); return *this; }
    Number &operator%=(const string &n) { m_data = (int)m_data % stoi(n); return *this; }
    Number &operator|=(const string &n) { m_data = (int)m_data | stoi(n); return *this; }
    Number &operator&=(const string &n) { m_data = (int)m_data & stoi(n); return *this; }
    Number &operator^=(const string &n) { m_data = (int)m_data ^ stoi(n); return *this; }
    Number &operator<<=(const string &n) { m_data = (int)m_data << stoi(n); return *this; }
    Number &operator>>=(const string &n) { m_data = (int)m_data >> stoi(n); return *this; }
    
    /* Number & char[] */
    Number operator+(const char *n) { return m_data + stod(n);}
    Number operator-(const char *n) { return m_data - stod(n);}
    
    Number operator/(const char *n) { return m_data / stod(n);}
    Number operator*(const char *n) { return m_data * stod(n);}
    Number operator%(const char *n) { return (int)m_data % stoi(n);}
    
    bool operator<(const char *n) { return m_data < stod(n); }
    bool operator>(const char *n) { return m_data > stod(n); }
    bool operator==(const char *n) { return m_data == stod(n); }
    bool operator!=(const char *n) { return m_data != stod(n); }
    
    bool operator&&(const char *n) { return m_data && stod(n); }
    bool operator||(const char *n) { return m_data || stod(n); }
    
    Number operator|(const char *n) { return (int)m_data | stoi(n); }
    Number operator&(const char *n) { return (int)m_data & stoi(n); }
    Number operator^(const char *n) { return (int)m_data ^ stoi(n); }
    Number operator<<(const char *n) { return (int)m_data << stoi(n); }
    Number operator>>(const char *n) { return (int)m_data >> stoi(n); }
    
    Number &operator+=(const char *n) { m_data += stod(n); return *this; }
    Number &operator-=(const char *n) { m_data -= stod(n); return *this; }
    Number &operator/=(const char *n) { m_data /= stod(n); return *this; }
    Number &operator*=(const char *n) { m_data *= stod(n); return *this; }
    Number &operator%=(const char *n) { m_data = (int)m_data % stoi(n); return *this; }
    Number &operator|=(const char *n) { m_data = (int)m_data | stoi(n); return *this; }
    Number &operator&=(const char *n) { m_data = (int)m_data & stoi(n); return *this; }
    Number &operator^=(const char *n) { m_data = (int)m_data ^ stoi(n); return *this; }
    Number &operator<<=(const char *n) { m_data = (int)m_data << stoi(n); return *this; }
    Number &operator>>=(const char *n) { m_data = (int)m_data >> stoi(n); return *this; }
    
    /* Common cases */
    /*post*/ Number operator++(int) { return m_data++;}
    /*pre*/ Number &operator++() { ++m_data; return *this;}
    /*post*/ Number operator--(int) { return m_data--;}
    /*pre*/ Number &operator--() { --m_data; return *this;}
    Number operator+() { return m_data; }
    Number operator-() { return -m_data; }
    
    bool operator!() { return !m_data; }
    bool operator~() { return ~(int)m_data; }
    
    operator int() const { return (int)m_data; }
    operator double() const { return m_data; }
    operator float() const { return (float)m_data; }
    operator string() const { return to_string(m_data); }
};

/* string -> Number */
Number operator+(const string &str) { return stod(str); }
Number operator-(const string &str) { return stod(str); }

ostream& operator<<(ostream& os, const Number &num) {
    os << (double)num;
    return os;
}

int main() {
    Number num = 4;
    num <<= 1;
    cout << num << "\n";
    
    
    
    int lol = 5;
    If a = If(lol == 6);
    If not_a = !a;
    
    a.then([]{ cout << "kek\n"; })
    .other([]{ cout << "not kek\n"; })
    .other([]{ cout << "one more\n"; })
    .otherIf(lol < 8)
        .then([]{ cout << "loooool\n"; })
        .other([]{ cout << "not loooool\n"; });
        
    not_a.then([]{ cout << "not a lol\n"; });
    
    If b = a.otherIf(lol < 8);
    
    b.then([]{ cout << "loool\n"; });
    
    int i;
    For loop = For([&i]{ i = 0; }, If([&i]{ return i < 10; }), [&i]{ i++; });
    
    loop.then([&i]{
        cout << i << ". This is the LOOP\n";
    });
    loop.then([&i]{
        cout << i << ". This is the LOOP again\n";
    });
    
    Conditions c({
        {loop.share(), [&i]{
            cout << i << ". The last loop...\n";
        }},
        {new If([&lol]{return lol < 10;}), []{ cout << "lol < 10\n"; }},
        {new If([&lol]{return lol > 10;}), []{ cout << "lol > 10\n"; }},
        {new If([&lol]{return lol < 15;}), []{ cout << "lol < 15\n"; }},
        {new If([&lol]{return lol == 5;}), []{ cout << "lol == 5\n"; }},
        {new If([&lol]{return lol == 16;}),
            []{ cout << "lol == 16\n"; },
            []{ cout << "lol != 16\n"; }
        },
    });
    
    cout<<"==============\n";
    c.exec();
    cout<<"==============\n";
    lol = 16;
    c.exec();
    
    // int lol = 5, kek = 6;
    
    // //(lol < kek && kek < 10) || lol > 11
    
    // If test1([&lol, &kek]{return lol < kek;});
    // If test2([&lol, &kek]{return kek < 10;});
    // If test3([&lol, &kek]{return lol > 11;});
    // If wholeTest = (test1 & test2) | test3;
    
    // wholeTest.then([]{ cout << "o_o"; });
    // kek = 11;
    // wholeTest.then([]{ cout << "o_o"; });
    // lol = 15;
    // wholeTest.then([]{ cout << "o_o"; });

    
    return 0;
}
