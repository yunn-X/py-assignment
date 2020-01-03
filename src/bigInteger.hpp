//
//
#include <cstring>
#include <iostream>
#include <string>
#include <cmath>

#ifndef BIGINTEGER_BIGINTEGER_HPP
#define BIGINTEGER_BIGINTEGER_HPP

class bigInteger{
    friend bool operator>(const bigInteger &a, const bigInteger &b);
    friend bool operator>=(const bigInteger &a, const bigInteger &b);
    friend bool operator<(const bigInteger &a, const bigInteger &b);
    friend bool operator<=(const bigInteger &a, const bigInteger &b);
    friend bool operator==(const bigInteger &a, const bigInteger &b);
    friend bool operator!=(const bigInteger &a, const bigInteger &b);
    friend std::istream &operator>>(std::istream & in, bigInteger &bi);
    friend std::ostream &operator<<(std::ostream & os, const bigInteger &bi);
    friend bigInteger operator+(const bigInteger &a, const bigInteger &b);
    friend bigInteger operator-(const bigInteger &a, const bigInteger &b);
    friend bigInteger operator*(const bigInteger &a, const bigInteger &b);
    friend bigInteger operator/(const bigInteger &a, const bigInteger &b);
    friend bigInteger operator%(const bigInteger &a, const bigInteger &b);

private:
    char *bigint = nullptr;
    unsigned long long size = 0;
    int sign = 1;//-1 = '-';
    static bigInteger buling (const bigInteger &tp, long long zeronum);

public:
    bigInteger() {}
    bigInteger (const char *input)
    {
        size = strlen(input);
        bigint = new char [size + 10];
        unsigned long long i = 0;
        if (input[0] == '-')
        {
            sign = -1;
            --size;
            ++i;
        }
        else sign = 1;
        for (unsigned long long j = 0;i < strlen(input);++i,++j) bigint[j] = input[i];
        bigint[size] = '\0';
    }
    bigInteger (const bigInteger &bi)
    {
        size = bi.size;
        sign = bi.sign;
        bigint = new char [size + 10];
        for (unsigned long long i = 0;i < bi.size;++i)
        {
            bigint[i] = bi.bigint[i];
        }
        bigint[size] = '\0';
    }
    bigInteger (const int *c, unsigned long long length, int sg)
    {
        sign = sg;
        size = length;
        bigint = new char [size + 10];
        long long i = length - 1,j = 0;
        for (;i >= 0;--i,++j)
        {
            bigint[j] = static_cast<char> (c[i] + '0');
        }
        bigint[size] = '\0';
    }
    bigInteger (const int *c, unsigned long long length, int sg, int m)
    {
        sign = sg;
        size = length;
        bigint = new char [size + 10];
        long long i = 0,j = 0;
        for ( ;i < length;++i,++j)
        {
            bigint[j] = static_cast<char> (c[i] + '0');
        }
        bigint[size] = '\0';
    }
    explicit operator double () const
    {
        double db;
        db = atof(bigint);
        if(sign == -1) db = -db;
        return db;
    }
    explicit operator bool () const
    {
        return strcmp(bigint, "0") != 0;
    }
    explicit operator std::string() const
    {
        std::string st;
        if (sign < 0)
        {
            std::string temp = "-";
            st = temp + bigint;
        }
        else st = bigint;
        return (st);
    }
    bigInteger &operator=(const bigInteger &b)
    {
        if (this == &b) return *this;
        delete[]bigint;
        size = b.size;
        sign = b.sign;
        bigint = new char [size + 10];
        for (unsigned long long i = 0;i < b.size;++i)
        {
            bigint[i] = b.bigint[i];
        }
        bigint[size] = '\0';
        return *this;
    }
    bigInteger &operator+=(const bigInteger &b)
    {
        *this = *this + b;
        return *this;
    }
    bigInteger &operator-=(const bigInteger &b)
    {
        *this = *this - b;
        return *this;
    }
    bigInteger &operator*=(const bigInteger &b)
    {
        *this = *this * b;
        return *this;
    }
    bigInteger &operator/=(const bigInteger &b)
    {
        *this = *this / b;
        return *this;
    }
    ~bigInteger()
    {
        delete []bigint;
    }
};

#endif //BIGINTEGER_BIGINTEGER_HPP

bool operator>(const bigInteger &a, const bigInteger &b)
{
    if (a.sign == 1 && b.sign == -1) {
        return true;
    }
    if (a.sign == -1 && b.sign == 1) {
        return false;
    }
    if (a.sign == 1 && b.sign == 1)
    {
        if (a.size > b.size) {
            return true;
        }
        if (a.size == b.size)
        {
            for (int m = 0; m < a.size; ++m)
            {
                if (a.bigint[m] > b.bigint[m])
                {
                    return true;
                }
                if (a.bigint[m] < b.bigint[m])
                {
                    return false;
                }
            }
        }
        return false;
    }
    else
    {
        if (a.size < b.size) {
            return true;
        }
        if (a.size == b.size)
        {
            for (int m = 0;m < a.size;++m)
            {
                if (a.bigint[m] < b.bigint[m])
                {
                    return  true;
                }
                if (a.bigint[m] > b.bigint[m])
                {
                    return false;
                }
            }
        }
        return false;
    }
}

bool operator<(const bigInteger &a, const bigInteger &b)
{
    if (a.sign == 1 && b.sign == -1) return false;
    if (a.sign == -1 && b.sign == 1) return true;
    if (a.sign == 1 && b.sign == 1)
    {
        if (a.size < b.size) {
            return true;
        }
        if (a.size == b.size)
        {
            for (int m = 0;m < a.size;++m)
            {
                if (a.bigint[m] < b.bigint[m])
                {
                    {
                        return  true;
                    }
                }
                if (a.bigint[m] > b.bigint[m])
                {
                    return false;
                }
            }
        }
        return false;
    }
    else
    {
        if (a.size > b.size) {
            return true;
        }
        if (a.size == b.size)
        {
            for (int m = 0; m < a.size; ++m)
            {
                if (a.bigint[m] > b.bigint[m])
                {
                    return true;
                }
            }
        }
        return false;
    }
}

bool operator==(const bigInteger &a, const bigInteger &b)
{
    if (a.sign != b.sign) {
        return false;
    }
    if (a.size != b.size)
    {
        return false;
    }
    if (a.size == b.size)
    {
        for (long long m = 0;m < a.size;++m)
        {
            if (a.bigint[m] != b.bigint[m])
            {
                return  false;
            }
        }
    }
    return true;
}

bool operator!=(const bigInteger &a, const bigInteger &b)
{
    return !(a == b);
}

bool operator>=(const bigInteger &a, const bigInteger &b)
{
    return !(a < b);
}

bool operator<=(const bigInteger &a, const bigInteger &b)
{
    return !(a > b);
}

std::istream &operator>>(std::istream &in,bigInteger &bi)
{
    std::string str;
    in >> str;
    bi.size = str.length();
    bi.bigint = new char [bi.size + 10];
    unsigned long long i = 0;
    if (str[0] == '-')
    {
        bi.sign = -1;
        --bi.size;
        ++i;
    }
    else bi.sign = 1;
    for (unsigned long long j = 0;i < str.length();++i,++j) bi.bigint[j] = str[i];
    bi.bigint[bi.size] = '\0';
    return in;
}


std::ostream &operator<<(std::ostream & os, const bigInteger &bi)
{
    if (bi.sign == -1)
    {
        os << '-';
    }
    os << bi.bigint;
    return os;
}

bigInteger bigInteger::buling (const bigInteger &tp,long long zeronum)
{
    bigInteger temp = tp;
    temp.size += zeronum;
    delete []temp.bigint;
    temp.bigint = new char[temp.size + 10];
    for (long long i = 0;i <temp.size;++i)
    {
        if (i < tp.size) temp.bigint[i] = tp.bigint[i];
        else temp.bigint[i] = '0';
    }
    temp.sign = 1;
    temp.bigint[temp.size] = '\0';
    return temp;
}


bigInteger operator+(const bigInteger &a, const bigInteger &b)
{

    int sg;
    if (a.sign == -1 && b.sign == -1) sg = -1;
    else if (a.sign == 1 && b.sign == 1) {
        sg = 1;
    }
    else
    {
        bigInteger temp = b;
        temp.sign = -b.sign;
        bigInteger tp = a - temp;
        return tp;
    }
    unsigned long long length = (a.size > b.size)? a.size : b.size;
    int *c = new int [length + 10];
    for (long long ti = 0;ti < length + 10;++ti) c[ti] = 0;
    long long i,j,k = 0;
    int r = 0;

    for (i = (a.size - 1),j = (b.size - 1);(i >= 0) && (j >= 0);--i,--j)
    {
        int p = (a.bigint[i] - '0') + (b.bigint[j] - '0') + r;
        r = p / 10;
        c [k++] = p % 10;
    }

    while (i >= 0)
    {
        int p = (a.bigint[i] - '0') + r;
        r = p / 10;
        c [k++] = p % 10;
        i--;
    }
    while (j >= 0)
    {
        int p = (b.bigint[j] - '0') + r;
        r = p / 10;
        c[k++] = p % 10;
        j--;
    }
    if (r != 0)
    {
        c[k++] = r;
    }
    bigInteger tp (c, k, sg);
    delete []c;
    return tp;
}

bigInteger operator-(const bigInteger &a, const bigInteger &b)
{
    int sg;
    if (a.sign == -1 && b.sign == -1) sg = -1;
    else if (a.sign == 1 && b.sign == 1) sg = 1;
    else
    {
        bigInteger temp = b;
        temp.sign = -b.sign;
        bigInteger tp = a + temp;
        return tp;
    }
    bigInteger a1 = a,b1 = b;
    a1.sign = b1.sign = 1;
    unsigned long long length = (a.size > b.size)? a.size : b.size;
    int *c = new int [length + 10];
    for (long long ti = 0;ti < length + 10;++ti) c[ti] = 0;
    long long i, j, k = 0;
    int rent = 0;
    bool flag = true;
    if (b1 > a1) flag = false;
    for (i = a.size - 1, j = b.size - 1;i >= 0 && j >= 0 ;--i,--j)
    {
        int tmp1,tmp2;
        if (flag)
        {
            tmp1 = a.bigint[i] - '0';
            tmp2 = b.bigint[j] - '0';
        }
        else
        {
            tmp1 = b.bigint[j] - '0';
            tmp2 = a.bigint[i] - '0';
        }
        if (tmp1 - rent >= tmp2)
        {
            c[k++] = tmp1 - tmp2 - rent;
            rent = 0;
        }
        else
        {
            c[k++] = 10 + tmp1 - tmp2 - rent;
            rent = 1;
        }
    }
    if (flag)
    {
        if (rent == 1)
        {
            while(true)
            {
                if (a.bigint[i] - rent >= '0')
                {
                    c[k++] = a.bigint[i] - '0' - rent;
                    --i;
                    break;
                }
                else
                {
                    c[k++] = 10 + a.bigint[i] - '0' - 1;
                    --i;
                }
            }
        }
        while (i >= 0)
        {
            c[k++] = a.bigint[i] - '0';
            --i;
        }
    }
    if (!flag)
    {
        if (rent == 1)
        {
            while(true)
            {
                if (b.bigint[j] - rent >= '0')
                {
                    c[k++] = b.bigint[j] - '0' - rent;
                    --j;
                    break;
                }
                else
                {
                    c[k++] = 10 + b.bigint[j] - '0' - 1;
                    --j;
                }
            }
        }
        while (j >= 0)
        {
            c[k++] = b.bigint[j] - '0';
            --j;
        }
    }
    if (!flag) sg = -sg;
    j = 0;
    long long m = k - 1;
    while (c[m] == 0 && m != 0) --m;
    bigInteger tp(c, m + 1, sg);
    delete []c;
    return tp;
}

bigInteger operator*(const bigInteger &a, const bigInteger &b)
{
    int sg;
    unsigned long long length = (a.size > b.size)? a.size : b.size;
    if (a.sign == b.sign) sg = 1;
    else sg = -1;
    int *c = new int[2 * length + 10];
    long long i,j, k = -1, t = 0;
    for (long long ti = 0;ti < length * 2 + 10;++ti) c[ti] = 0;
    int l,m;
    for (j = b.size - 1;j >= 0;--j)
    {
        ++k;
        t = k;
        m = 0;
        l = 0;
        for (i = a.size - 1;i >= 0;--i)
        {
            m = (b.bigint[j] - '0') * (a.bigint[i] - '0');
            l = m % 10;
            m /= 10;
            c[t + 1] += ((c[t] + l) / 10 + m);
            c[t] = (c[t] + l) % 10;
            ++t;
        }
    }
    while (c[t] / 10 > 0)
    {
        c[t + 1] = c[t] / 10;
        c[t] = c[t] % 10;
        ++t;
    }
    while (c[t] == 0 && t > 0) --t;
    bigInteger tp(c, t + 1, sg);
    if (strcmp(tp.bigint,"0") == 0) tp.sign = 1;
    delete []c;
    return tp;
}

bigInteger operator/(const bigInteger &a, const bigInteger &b)
{
    int sg;
    if (a.sign == b.sign) sg = 1;
    else sg = -1;
    bigInteger temp1 = a, temp3 = b, temp2 = b;
    temp1.sign = 1;
    temp2.sign = 1;
    long long ck = 0;
    int *c = new int[a.size + 10];
    bool flag = false;
    for (long long ti = 0;ti < a.size + 10;++ti) c[ti] = 0;
    long long delt = a.size - b.size;
    while (delt >= 0)
    {
        int k = 0;
        temp2 = bigInteger::buling(b, delt);
        if (temp1 >= temp2) flag = true;
        while (temp1 >= temp2)
        {
            ++k;
            temp1 = temp1 - temp2;
        }
        if(flag) c[ck++] = k;
        --delt;
    }
    if (ck == 0) ++ck;
    bigInteger tp(c, ck, sg, 1);
    bigInteger zero = "0",one = "1";
    if (temp1 != zero && sg == -1) {
        tp = tp - one;
    }
    if (strcmp(tp.bigint,"0") == 0) tp.sign = 1;
    delete []c;
    return tp;
}

bigInteger operator%(const bigInteger &a, const bigInteger &b)
{
    bigInteger tp = a - (a / b) * b;
    return tp;
}