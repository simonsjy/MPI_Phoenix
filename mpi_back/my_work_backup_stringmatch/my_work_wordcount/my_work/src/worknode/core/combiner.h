/*========================================================================
#   FileName: combiner.h
#     Author: wangjian
#      Email: kuangrenyx@qq.com
#    Version: 1.0.0
#   HomePage: http://www.xxxxxxxxx.cn
# LastChange: 2013-10-24 12:13:18
========================================================================*/

#ifndef COMBINER_H_
#define COMBINER_H_

#include <vector>

// The assumption with a combiner is that it will be very cheap to copy 
// (e.g. as cheap as a pointer or two)

// the buffer combiner does no combining, it just queues up the values 
// for the reducer.
template<typename V, template<class> class Allocator = std::allocator>
class buffer_combiner
{
    std::vector<V, Allocator<V> >* data;

public:    
    buffer_combiner() : data(new std::vector<V, Allocator<V> >) {}
    void add(V const& v) {
        data->push_back(v);
    }

    bool empty() const {
        return data->size() == 0;
    }

    class combined
    {
        std::vector< std::vector<V, Allocator<V> >*, 
            Allocator<std::vector<V, Allocator<V> >* > > items;
        mutable unsigned int current_list, current_index;
    public:
        combined() : current_list(0), current_index(0) {}

        void add(buffer_combiner<V, Allocator> const* c) {
            items.push_back(c->data);
        }

        bool next(V& v) const {
            if(current_list < items.size() && 
                current_index >= items[current_list]->size())
            {
                current_index = 0;
                current_list++;

                if((current_list+1) < items.size() && 
                    items[current_list+1]->size() > 0)
                    __builtin_prefetch (&items[current_list+1][0], 0, 1);
            }

            if(current_list < items.size() && 
                current_index < items[current_list]->size())
            {
                v = (*items[current_list])[current_index++];
                return true;
            }

            return false;
        }

        void reset() {
            current_list = 0;
            current_index = 0;
        }

        int size() const {
            return items.size();
        }

        void clear() {
            current_list = 0;
            current_index = 0;
            items.clear();
        }
    };

    void combineinto(combined& m) const {
        m.add(*this);
    }
};

#ifndef MUST_REDUCE

template<class Impl, typename V, template<class> class Allocator = std::allocator>
class associative_combiner
{
public:
    V data;
    bool _empty;
public:
    associative_combiner() : _empty(true) {Impl::Init(data);}

    void add(V const& v) {
	Impl::F(data, v);
        _empty = false;
    }

    bool empty() const {
        return _empty;
    }

    class combined
    {
        V m;
        mutable int i;
        bool _empty;
    public:
        combined() : i(0), _empty(true) {Impl::Init(m);}
        void add(V const& v) { Impl::F(m, v); _empty = false; }
        void add(Impl const* c) {
	    if(!c->_empty) {
                Impl::F(m, c->data);
                _empty = false;
            }
        }
        bool next(V& v) const {
            v = m;
            return 0 == i++;
        }
        void reset() {
            i = 0;
        }
        int size() const {
            return _empty ? 0 : 1;
        }
        void clear() {
            Impl::Init(m);
            i = 0;
            _empty = true;
        }
    };

    void combineinto(combined& m) const {
        m.add(data);
    }
};

#else

template<class Impl, typename V, template<class> class Allocator = std::allocator>
class associative_combiner
{
    std::vector<V, Allocator<V> >* data;

public:    
    associative_combiner() : data(new std::vector<V, Allocator<V> >) {}
    void add(V const& v) {
        data->push_back(v);
    }

    bool empty() const {
        return data->size() == 0;
    }

    class combined
    {
        std::vector< std::vector<V, Allocator<V> >*, 
            Allocator<std::vector<V, Allocator<V> >* > > items;
        mutable bool done;
    public:
        combined() : done(false) {}

        void add(associative_combiner<Impl, V, Allocator> const* c) {
            items.push_back(c->data);
        }

        bool next(V& v) const {
            if(done) return false;
            
            V total;
            Impl::Init(total); 
            for(uint64_t current_list = 0; current_list < items.size(); 
                current_list++)
            {
                for(uint64_t i = 0; i < items[current_list]->size(); i++)
                    Impl::F(total, (*items[current_list])[i]);
            } 
            v = total;
            done = true;
            return true;
        }

        void reset() {
            done = false;
        }

        int size() const {
            return items.size();
        }

        void clear() {
            done = false;
            items.clear();
        }
    };

    void combineinto(combined& m) const {
        m.add(*this);
    }
};

#endif

template<class V, template<class> class Allocator = std::allocator>
class sum_combiner : public associative_combiner<sum_combiner<V, Allocator>, V, Allocator> 
{
public:
     static void F(V& a, V const& b) { a += b; }
     static void Init(V& a) { a = 0; }
};

template<class V, template<class> class Allocator = std::allocator>
class one_combiner : public associative_combiner<one_combiner<V, Allocator>, V, Allocator> 
{
public:
     static void F(V& a, V const& b) { a = b; }
     static void Init(V& a) {}
};

#endif /* COMBINER_H_ */

// vim: ts=8 sw=4 sts=4 smarttab smartindent
