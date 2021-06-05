// Online C++ compiler to run C++ program online
#include <iostream>
#include <functional>
#include <vector>

using namespace std;

class If {
    function<bool()> m_expr;
public:
    If(bool state): m_expr([state]{return state;}) {}
    If(const function<bool()> &expr): m_expr(expr) {}
    If(const If &o): m_expr(o.m_expr) {}
    If &operator=(const If &o) {
        m_expr = o.m_expr;
    }
    
    If &then(const function<void()> &fn){
        if(!m_expr()) return *this;
        fn();
        return *this;
    }
    
    If &other(const function<void()> &fn){
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
    If freeze(){ return If(m_expr()); }
};

class Conditions {
    typedef pair<If, vector<function<void()>>> condition_t;
    typedef vector<condition_t> conditions_t;
    conditions_t m_conditions;
public:
    Conditions(const conditions_t &conditions): m_conditions(conditions) {}
    Conditions &exec(){
        for(condition_t &c: m_conditions){
            auto &[cond, fns] = c;
            int size = fns.size();
            if(size == 0)
                continue;
            if(size == 1){
                cond.then(fns[0]);
                continue;
            }
            cond.then(fns[0]);
            cond.other(fns[1]);
        }
    }
};

int main() {
    // int lol = 5;
    // If a = If(lol == 6);
    // If not_a = !a;
    
    // a.then([]{ cout << "kek\n"; })
    // .other([]{ cout << "not kek\n"; })
    // .other([]{ cout << "one more\n"; })
    // .otherIf(lol < 8)
    //     .then([]{ cout << "loooool\n"; })
    //     .other([]{ cout << "not loooool\n"; });
        
    // not_a.then([]{ cout << "not a lol\n"; });
    
    // If b = a.otherIf(lol < 8);
    
    // b.then([]{ cout << "loool\n"; });
    
    // Conditions c = Conditions({
    //     {If([&lol]{return lol < 10;}).freeze(), {[]{ cout << "lol < 10\n"; }}},
    //     {If([&lol]{return lol > 10;}), {[]{ cout << "lol > 10\n"; }}},
    //     {If([&lol]{return lol < 15;}), {[]{ cout << "lol < 15\n"; }}},
    //     {If([&lol]{return lol == 5;}), {[]{ cout << "lol == 5\n"; }}},
    //     {If([&lol]{return lol == 16;}), {
    //         []{ cout << "lol == 16\n"; },
    //         []{ cout << "lol != 16\n"; }
    //     }},
    // });
    
    // cout<<"==============\n";
    // c.exec();
    // cout<<"==============\n";
    // lol = 16;
    // c.exec();
    
    int lol = 5, kek = 6;
    
    //(lol < kek && kek < 10) || lol > 11
    
    If test1([&lol, &kek]{return lol < kek;});
    If test2([&lol, &kek]{return kek < 10;});
    If test3([&lol, &kek]{return lol > 11;});
    If wholeTest = (test1 & test2) | test3;
    
    wholeTest.then([]{ cout << "o_o"; });
    kek = 11;
    wholeTest.then([]{ cout << "o_o"; });
    lol = 15;
    wholeTest.then([]{ cout << "o_o"; });

    
    return 0;
}
