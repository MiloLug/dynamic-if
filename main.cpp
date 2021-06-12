// Online C++ compiler to run C++ program online
#include <iostream>
#include <functional>
#include <vector>
#include <tuple>
#include <memory>
#include <list>
#include <initializer_list>
#include <string>
#include <map>

using namespace std;

typedef function<void()> void_fn;

class Object {
public:
    static string typeOf(Object *o){ return o->__class_name__(); }
    static bool hasParent(Object *o, const string &parent_name){ return false; }
    
    static string __s_class_name__(){ return "Object"; }
    virtual string __class_name__(){ return __s_class_name__(); }
    
    static shared_ptr<list<string>> __s_parents__(){ return shared_ptr<list<string>>(new list<string>({})); }
    virtual shared_ptr<list<string>> __parents__(){ return __s_parents__(); }
    
    virtual ~Object() {}
};

class Condition: public Object {
    list<void_fn> m_bindedThen = {};
    list<void_fn> m_bindedOther = {};
public:
    static string __s_class_name__(){ return "Condition"; }
    virtual string __class_name__(){ return __s_class_name__(); }
    
    static shared_ptr<list<string>> __s_parents__(){ 
        auto parents = Object::__s_parents__();
        parents->push_back(Object::__s_class_name__());
        return parents;
    }
    virtual shared_ptr<list<string>> __parents__(){ return __s_parents__(); }
    
    Condition() {}
    Condition(const Condition &o): m_bindedThen(o.m_bindedThen), m_bindedOther(o.m_bindedOther) {}
    Condition &operator=(const Condition &o) {
        m_bindedThen = o.m_bindedThen;
        m_bindedOther = o.m_bindedOther;
        return *this;
    }
    virtual shared_ptr<Condition> share() { return shared_ptr<Condition>(new Condition(*this)); }
    
    virtual Condition &then(const void_fn &fn) { return *this; }
    virtual Condition &other(const void_fn &fn) { return *this; }
    
    virtual Condition &bindThen(const void_fn &fn){
        m_bindedThen.push_back(fn);
        return *this;
    }
    virtual Condition &bindOther(const void_fn &fn){
        m_bindedOther.push_back(fn);
        return *this;
    }
    virtual Condition &exec(){
        for(auto &fn: m_bindedThen) this->then(fn);
        for(auto &fn: m_bindedOther) this->other(fn);
        return *this;
    }

    virtual ~Condition() {}
};

class If: public Condition {
    function<bool()> m_expr;
public:
    static string __s_class_name__(){ return "If"; }
    virtual string __class_name__(){ return __s_class_name__(); }
    
    static shared_ptr<list<string>> __s_parents__(){ 
        auto parents = Condition::__s_parents__();
        parents->push_back(Condition::__s_class_name__());
        return parents;
    }
    virtual shared_ptr<list<string>> __parents__(){ return __s_parents__(); }

    If(bool state): m_expr([state]{return state;}) {}
    If(const function<bool()> &expr): m_expr(expr) {}
    If(const If &o): m_expr(o.m_expr), Condition((Condition)o) {}
    If &operator=(const If &o) {
        m_expr = o.m_expr;
        Condition::operator=((Condition)o);
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
    
    virtual If &bindThen(const void_fn &fn){ Condition::bindThen(fn); return *this; }
    virtual If &bindOther(const void_fn &fn){ Condition::bindOther(fn); return *this; }
    virtual If &exec(){ Condition::exec(); return *this; }
    
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
    bool m_hasInit;
    If m_condition;
    void_fn m_postExpr;
    bool m_hasPostExpr;
    bool m_stateBreak = false;
    bool m_forever;
public:
    static string __s_class_name__(){ return "For"; }
    virtual string __class_name__(){ return __s_class_name__(); }
    
    static shared_ptr<list<string>> __s_parents__(){ 
        auto parents = Condition::__s_parents__();
        parents->push_back(Condition::__s_class_name__());
        return parents;
    }
    virtual shared_ptr<list<string>> __parents__(){ return __s_parents__(); }

    For(const void_fn &init, const If &condition, const void_fn &postExpr):
        m_init(init), m_hasInit(true), m_condition(condition),
        m_postExpr(postExpr), m_hasPostExpr(true), m_forever(false) {}
    For(const void_fn &init, const If &condition):
        m_init(init), m_hasInit(true), m_condition(condition),
        m_postExpr([]{}), m_hasPostExpr(false), m_forever(false) {}
    For(const If &condition):
        m_init([]{}), m_hasInit(false), m_condition(condition),
        m_postExpr([]{}), m_hasPostExpr(false), m_forever(false) {}
    For():
        m_init([]{}), m_hasInit(false), m_condition(true),
        m_postExpr([]{}), m_hasPostExpr(false), m_forever(true) {}
        
    For(const For &o):
        m_init(o.m_init), m_hasInit(o.m_hasInit), m_condition(o.m_condition),
        m_postExpr(o.m_postExpr), m_hasPostExpr(o.m_hasPostExpr),
        m_forever(o.m_forever), Condition((Condition)o) {}
    For &operator=(const For &o) {
        m_init = o.m_init;
        m_hasInit = o.m_hasInit;
        m_condition = o.m_condition;
        m_postExpr = o.m_postExpr;
        m_hasPostExpr = o.m_hasPostExpr;
        m_forever = o.m_forever;
        Condition::operator=((Condition)o);
        return *this;
    }
    virtual shared_ptr<Condition> share() { return shared_ptr<Condition>(new For(*this)); }
    
    virtual For &then(const void_fn &fn){
        m_stateBreak = false;
        if(m_hasInit) m_init();
        while(!m_stateBreak && (m_forever || (bool)m_condition)){
            fn();
            if(m_hasPostExpr) m_postExpr();
        }
        return *this;
    }
    virtual For &other(const void_fn &fn){ return *this; }
    
    virtual For &bindThen(const void_fn &fn){ Condition::bindThen(fn); return *this; }
    virtual For &bindOther(const void_fn &fn){ Condition::bindOther(fn); return *this; }
    virtual For &exec(){ Condition::exec(); return *this; }
    
    For &stop(){ m_stateBreak = true; return *this; }
    
    virtual ~For() {}
};

class ConditionExecutor: public Object {
    shared_ptr<Condition> m_condition;
    void_fn m_thenFn, m_otherFn;
public:
    static string __s_class_name__(){ return "ConditionExecutor"; }
    virtual string __class_name__(){ return __s_class_name__(); }
    
    static shared_ptr<list<string>> __s_parents__(){ 
        auto parents = Object::__s_parents__();
        parents->push_back(Object::__s_class_name__());
        return parents;
    }
    virtual shared_ptr<list<string>> __parents__(){ return __s_parents__(); }

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

    virtual ~ConditionExecutor() {}
};

class Conditions: public Object {
    typedef vector<ConditionExecutor> conditions_t;
    vector<ConditionExecutor> m_conditions;
public:
    static string __s_class_name__(){ return "Conditions"; }
    virtual string __class_name__(){ return __s_class_name__(); }
    
    static shared_ptr<list<string>> __s_parents__(){ 
        auto parents = Object::__s_parents__();
        parents->push_back(Object::__s_class_name__());
        return parents;
    }
    virtual shared_ptr<list<string>> __parents__(){ return __s_parents__(); }

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


class Number: public Object {
    double m_data;
public:
    static string __s_class_name__(){ return "Number"; }
    virtual string __class_name__(){ return __s_class_name__(); }
    
    static shared_ptr<list<string>> __s_parents__(){ 
        auto parents = Object::__s_parents__();
        parents->push_back(Object::__s_class_name__());
        return parents;
    }
    virtual shared_ptr<list<string>> __parents__(){ return __s_parents__(); }

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

class Function: public Object{
    function<void*(void**)> m_base;
public:
    static string __s_class_name__(){ return "Function"; }
    virtual string __class_name__(){ return __s_class_name__(); }
    
    static shared_ptr<list<string>> __s_parents__(){ 
        auto parents = Object::__s_parents__();
        parents->push_back(Object::__s_class_name__());
        return parents;
    }
    virtual shared_ptr<list<string>> __parents__(){ return __s_parents__(); }

    Function(const function<void*(void**)> &base): m_base(base) {}
    Function(const Function &o): m_base(o.m_base) {}
    Function &operator=(const Function &o) {
        m_base = o.m_base;
        return *this;
    }
    
    void* operator()(void **args = nullptr){
        return m_base(args);
    }
    
    virtual ~Function() {}
};

/* string -> Number */
Number operator+(const string &str) { return stod(str); }
Number operator-(const string &str) { return stod(str); }

ostream& operator<<(ostream& os, const Number &num) {
    os << (double)num;
    return os;
}

int main() {
    Function gg([](void**){
        return new Number(33);
    });
    Number *gghh = (Number*)gg();
    Number num = 4;
    num <<= 1;
    cout << num << " : " << *gghh << "\n";
    
    auto l = For::__s_parents__();
    list<string>::iterator s;
    
    For lolkek = For([&]{ s = l->begin(); }, If([&]{ return s != l->end(); }), [&]{ s++; })
    .bindThen([&]{
        cout << *s << "/";
    })
    .exec();
    cout << endl;
    
    l = gg.__parents__();
    lolkek.exec();
    cout << endl;
    
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
