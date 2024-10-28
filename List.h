#ifndef LIST_H_INCLUDED
#define LIST_H_INCLUDED
#include <iostream>
template <typename T>
//template <class... Args>    /// шаблонный параметр для передачи любого кол-ва аргументов
class cList
{
public:
    cList() : _first(nullptr), _last(nullptr), _current(nullptr), count(0)
    {

    }
    ~cList()
    {
        clear();
    }
    void clear()
    {
        Node* toDel = _last;
        while (toDel)
        {
            _last = toDel->prev;
            DelNode(toDel);
            toDel = _last;
        }
    }
    void push_back(const T& val)
    {
        Node* newNode = new Node(val);
        AddNode(newNode);
    }
    //template <class... Args>    /// шаблонный параметр для передачи любого кол-ва аргументов
    template <typename... Args>
    void emplace_back(const Args&... args) /// WORK ?
    {
        Node* newNode = new Node(args...);
        AddNode(newNode);
    }
    int erase(int it) // удаляет по интератору, возвращает следующий не удалённый. НЕ путать с id
    {
        int steps = idFromIter(it) - _first->id;
        Node* toDel = _first;
        int ret = 0; /// end
        while (steps--)
            toDel = toDel->next;
        if (toDel->prev)
        {
            toDel->prev->next = toDel->next;
        }
        else
        {
            _first = toDel->next;
        }
        Node* tail = toDel->next;
        while (tail)
        {
            tail->id--;
            tail = tail->next;
        }
        if (toDel->next)
        {
            ret = toDel->next->id;
            toDel->next->prev = toDel->prev;
        }
        else
        {
            _last = toDel->prev;
        }
        delete toDel;
        --count;
        return iterFromId(ret);// -1 конец
    }
    T* erase(T* pDel) // удаляет по интератору, возвращает следующий не удалённый. НЕ путать с id
    {
        T* pRet = nullptr;
        Node* toDel = _first;
        while (toDel && pDel != &(toDel->value))
            toDel = toDel->next;
        if (toDel->prev)
        {
            toDel->prev->next = toDel->next;
        }
        else
        {
            _first = toDel->next;
        }
        Node* tail = toDel->next;
        while (tail)
        {
            tail->id--;
            tail = tail->next;
        }
        if (toDel->next)
        {
            pRet = &(toDel->next->value);
            _current = toDel->next;
            toDel->next->prev = toDel->prev;
        }
        else
        {
            _last = toDel->prev;
        }
        delete toDel;
        --count;
        //if (pRet) _current = pRet;
        return pRet;
    }
    size_t size()
    {
        return count;
    }
    T& back() // error if list is blank
    {
        return _last->value;
    }
    T& front()// error if list is blank
    {
        return _first->value;
    }
    T* begin()
    {
        _current = _first;
        return _current ? &(_current->value) : nullptr;
    }
    T* last()
    {
        _current = _last;
        return _current ? &(_current->value) : nullptr;
    }
    T* next()
    {
        _current = _current->next;
        return _current ? &(_current->value) : nullptr;
    }
    T* prev()
    {
        _current = _current->prev;
        return _current ? &(_current->value) : nullptr;
    }
    T* find(const T& val)
    {
        T* pRet = nullptr;
        Node* start = _first;
        while (start)
        {
            if (start->value == val)
            {
                pRet = &(start->value);
                start = nullptr;
            }
            else
            {
                start = start->next;
            }
        }
        return pRet;
    }
    size_t empty()
    {
        return count == 0;
    }
    void* get_first()
    {
        return reinterpret_cast<void*>(_first);
    }
    void* get_last()
    {
        return reinterpret_cast<void*>(_last);
    }
    void clear_after_move()
    {
        count = 0;
        _first = _last = nullptr;
    }
    void splice(cList<T>& from)
    {
        if (from.empty()) return;
        Node* first = reinterpret_cast<Node*>(from.get_first());
        if (empty())
        {
            _first = first;
        }
        else
        {
            _last->next = first;
            first->prev = _last;
            while (first)
            {
                first->id = first->prev->id + 1;
                first = first->next;
            }
        }
        _last = reinterpret_cast<Node*>(from.get_last());
        //from.clear_after_move();
        count += from.size();
        from.clear_after_move();
    }

    void sort()
    {
        /// TODO ???
    }
    /// to delete ///
    void Print()
    {
        if (_first)
        {
            Node* tmp = _first;
            while (tmp)
            {
                PrintNode(tmp);
                tmp = tmp->next;
            }
        }
    }
    ///
private:
    //template <class... Args>    /// шаблонный параметр для передачи любого кол-ва аргументов
    struct Node
    {
        Node(const T& val) : value(val), next(nullptr), prev(nullptr)
        {
        }
        template <typename... Args> /// шаблонный параметр для передачи любого кол-ва аргументов
        Node(const Args&... args) : value(args...), next(nullptr), prev(nullptr)
        {
            //value = new T(args...);
        }
        T value;
        Node* next;
        Node* prev;
        int id;
    };
    Node* _first;
    Node* _last;
    Node* _current;
    size_t count;
    void DelNode(Node* pNode)
    {
        if (pNode)
            delete pNode;
    }
    void AddNode(Node* newNode)
    {
        if (_last)
        {
            newNode->id = _last->id + 1;
            newNode->prev = _last;
            _last->next = newNode;
            _last = newNode;
        }
        else
        {
            _current = _last = _first = newNode;
            //_first->next = _last;   /// Maybe not need - зацикливание!!!!!
            //_last->prev = _first;   ///
            _first->id = 1; /// начало итерирования с 1 (?)
        }
        if (!_current)
            _current = _first;
        ++count;
    }
    /// to delete ///
    void PrintNode(Node* pNode)
    {
        if (pNode)
        {
            std::cout << "Node# " << pNode->id << " val: " << pNode->value << std::endl;
        }
    }
    int iterFromId(int id)
    {
        return --id;
    }
    int idFromIter(int iter)
    {
        return ++iter;
    }
};


#endif // LIST_H_INCLUDED
