#ifndef VECTOR_H_INCLUDED
#define VECTOR_H_INCLUDED
#include <iostream> /// temp
template <typename T>
class cVector
{
public:
    cVector(size_t n = 0)
    {
        if (!n) n = 3;   // ���� �� ���� �������
        _size = 0;
        _capacity = n;
        _arr = _New(n); /// ��������� "�����" ������ ��� n ��������
    }
    cVector(const cVector& from) : _size(from._size), _capacity(from._capacity), _iterator(from._iterator)
    { /// ����������� ����������� (�������� ����� �������)
        _arr = _New(_capacity);
        for (size_t i = 0; i < _size; ++i)
        {
            new(_arr + i) T(from._arr[i]); // ����� ����� ������������ ����������� (����������� ��������� �� from._arr � _arr ������� ���������� ������)
        }
    }
    ~cVector(void)
    {
        _Delete();
    }
    void reserve(size_t n) // �������������� n-��������� ������������� �������
    {
        if (n <= _capacity) return;  /// ���-�� ��� ��������, �������� � ������������. ��������� ������������ ���������/������������ ������
        T* arrnew = _New(n); // ����� ������ �� n-���������
        for (size_t i = 0; i < _size; ++i)
        {
            new(arrnew + i) T(_arr[i]); // ����� ����� ������������ ����������� (����������� ��������� �� _arr � arrnew)
        }
        _Delete(); // ������� ���������� _arr
        _arr = arrnew;
        _capacity = n;
    }
    void push_back(const T& val)
    {
        if (_capacity == _size) reserve(2 * _size);
        new(_arr + _size) T(val); // ����� ����� ������������ �����������
        ++_size;
    }
    bool empty(void) const    /// + const ?
    {
        return _size == 0;
    }
    size_t size(void) const  /// + const ?
    {
        return _size;
    }
    T& front(void) const // const ?
    {
        return _arr[0];
    }
    T& back(void) const  // const ?
    {
        return _arr[_size - 1];
    }
    void pop_back(void)
    {
        if (empty()) return;
        --_size;
        (_arr - _size)->~T(); // (_arr + _size)->~T();
        //--_size;
    }
    size_t consist(const T& val)
    {
        size_t cons = 0;
        while (cons < _size && _arr[cons] != val)
        {
            ++cons;
        }
        return cons;
    }
    template <typename... Args>    /// ��������� �������� ��� �������� ������ ���-�� ����������
    void emplace_back(const Args&... args) // ��� ��� (Args&&... args)
    {
        if (_capacity == _size) reserve(2 * _size);
        new(_arr + _size) T(args...); // ����� ������������ ������ T �� ������� ���������� args
        // _arr + _size = new T(args...);
        ++_size;
    }
    ///
    void print(void)    /// temp for test only with simple types
    {
        for (size_t i = 0; i < _size; ++i)
        {
            std::cout << _arr[i] << " ";
        }
        std::cout << "cap: " << _capacity << std::endl;
    }
    ///
    T& operator[](size_t n)
    {
        return _arr[n];
    }
    T& operator[](size_t n) const
    {
        return _arr[n];
    }
private:
    T* _arr; // ������������ ������ ������
    size_t _size;  // ���-�� ������� ���������
    size_t _capacity;   // ������� ������� ������������� �������
    size_t _iterator; // not used yet

    T* _New(size_t n)  // ��������� ������ ��� n ��������
    {
        return reinterpret_cast<T*>(operator new(n * sizeof(T)));
        //return reinterpret_cast<T*>(new char[n * sizeof(T)]);
    }
    void _Delete() // �������� ���� ��������� � ������� � ������������ ������
    {
        if (_arr)
        {
            for (size_t i = 0; i < _size; ++i)
            {
                (_arr + i)->~T(); // ����� ����� �����������, ������ ��� ��������� ��������
            }
            //if (_size < _capacity)
                //operator delete(reinterpret_cast<void*>(_arr), _capacity * sizeof(T));
            operator delete(reinterpret_cast<void*>(_arr), _capacity * sizeof(T));
            _arr = nullptr;
            //delete[] reinterpret_cast<char*>(_arr);
        }
    }
};


#endif // VECTOR_H_INCLUDED
