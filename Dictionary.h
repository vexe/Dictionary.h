
#define ArrayCount(Array) sizeof(Array) / sizeof(*Array)

typedef char* string;

static int PrimeHelper_Primes[] = 
{
    3,
    7,
    11,
    17,
    23,
    29,
    37,
    47,
    59,
    71,
    89,
    107,
    131,
    163,
    197,
    239,
    293,
    353,
    431,
    521,
    631,
    761,
    919,
    1103,
    1327,
    1597,
    1931,
    2333,
    2801,
    3371,
    4049,
    4861,
    5839,
    7013,
    8419,
    10103,
    12143,
    14591,
    17519,
    21023,
    25229,
    30293,
    36353,
    43627,
    52361,
    62851,
    75431,
    90523,
    108631,
    130363,
    156437,
    187751,
    225307,
    270371,
    324449,
    389357,
    467237,
    560689,
    672827,
    807403,
    968897,
    1162687,
    1395263,
    1674319,
    2009191,
    2411033,
    2893249,
    3471899,
    4166287,
    4999559,
    5999471,
    7199369

};

struct hash_table
{
    #define NUM 128

    int _Buckets[NUM];
    int _HashCodes[NUM];
    int _Next[NUM];
    int _Count;
    int _FreeList;
    int _FreeCount;

    int _Keys[NUM];
    string _Values[NUM];

    int(*CompareKeys)(int, int);
    int(*CompareValues)(string, string);
    int(*GetHashCode)(int);
    
    int Count()
    {
        return _Count - _FreeCount;
    }

    string operator () (const int& Key)
    {
        int index = FindIndex(Key);
        if (index >= 0)
            return _Values[index];
        assert(false);
        return {};
    }

    void operator () (const int& Key, const string& Value)
    {
        Insert(Key, Value, false);
    }

    static hash_table Create()
    {
        return Create(0, null);
    }

    static hash_table Create(int Capacity)
    {
        return Create(Capacity, null);
    }

    static hash_table Create(int (*CompareKeys)(int, int))
    {
        return Create(0, CompareKeys);
    }

    static hash_table Create(int capacity, int(*CompareKeys)(int, int))
    {
        assert(capacity >= 0);

        hash_table Table;
        Table.Initialize(capacity);
        Table.CompareKeys = CompareKeys;
        Table._Count = 0;
        return Table;
    }

    bool ContainsValue(string value)
    {
        if (value == null)
        {
            for (int i = 0; i < _Count; i++)
            {
                if (_HashCodes[i] >= 0 && _Values[i] == null)
                    return true;
            }
        }
        else
        {
            for (int i = 0; i < _Count; i++)
            {
                if (_HashCodes[i] >= 0 && CompareValues(_Values[i], value) == 0)
                    return true;
            }
        }
        return false;
    }

    bool ContainsKey(int key)
    {
        return FindIndex(key) >= 0;
    }

    int GetKeysLength()
    {
        return NUM;
    }

    int GetBucketsLength()
    {
        return NUM;
    }

    void Clear()
    {
        if (_Count <= 0)
            return;

        // @NotSure: what ot use for buckets length?
        for (int i = 0; i < GetBucketsLength(); i++)
            _Buckets[i] = -1;

        /*Array.Clear(_Keys, 0, _Count);
        Array.Clear(_Values, 0, _Count);
        Array.Clear(_HashCodes, 0, _Count);
        Array.Clear(_Next, 0, _Count);
*/
        _FreeList = -1;
        _Count = 0;
        _FreeCount = 0;
    }

    void Add(int key, string value)
    {
        Insert(key, value, true);
    }
/*
    void Resize(int newSize, bool forceNewHashCodes)
    {
        int[] bucketsCopy = new int[newSize];
        for (int i = 0; i < bucketsCopy.Length; i++)
            bucketsCopy[i] = -1;

        var keysCopy = new TKey[newSize];
        var valuesCopy = new TValue[newSize];
        var hashCodesCopy = new int[newSize];
        var nextCopy = new int[newSize];

        Array.Copy(_Values, 0, valuesCopy, 0, _Count);
        Array.Copy(_Keys, 0, keysCopy, 0, _Count);
        Array.Copy(_HashCodes, 0, hashCodesCopy, 0, _Count);
        Array.Copy(_Next, 0, nextCopy, 0, _Count);

        if (forceNewHashCodes)
        {
            for (int i = 0; i < _Count; i++)
            {
                if (hashCodesCopy[i] != -1)
                    hashCodesCopy[i] = (_Comparer.GetHashCode(keysCopy[i]) & 2147483647);
            }
        }

        for (int i = 0; i < _Count; i++)
        {
            int index = hashCodesCopy[i] % newSize;
            nextCopy[i] = bucketsCopy[index];
            bucketsCopy[index] = i;
        }

        _Buckets = bucketsCopy;
        _Keys = keysCopy;
        _Values = valuesCopy;
        _HashCodes = hashCodesCopy;
        _Next = nextCopy;
    }

    void Resize()
    {
        Resize(PrimeHelper_ExpandPrime(_Count), false);
    }
*/
    bool Remove(int key)
    {
        int hash = GetHashCode(key) & 2147483647;
        int index = hash % GetBucketsLength();
        int num = -1;
        for (int i = _Buckets[index]; i >= 0; i = _Next[i])
        {
            if (_HashCodes[i] == hash && CompareKeys(_Keys[i], key) == 0)
            {
                if (num < 0)
                    _Buckets[index] = _Next[i];
                else
                    _Next[num] = _Next[i];

                _HashCodes[i] = -1;
                _Next[i] = _FreeList;
                _Keys[i] = {};
                _Values[i] = {};
                _FreeList = i;
                _FreeCount++;
                return true;
            }
            num = i;
        }
        return false;
    }

    void Insert(int key, string value, bool add)
    {
        if (_Buckets == null)
            Initialize(0);

        int hash = GetHashCode(key) & 2147483647;
        int index = hash % GetBucketsLength();
        int num1 = 0;
        for (int i = _Buckets[index]; i >= 0; i = _Next[i])
        {
            if (_HashCodes[i] == hash && CompareKeys(_Keys[i], key) == 0)
            {
                assert(!add);
                _Values[i] = value;
                return;
            }
            num1++;
        }
        int num2;
        if (_FreeCount > 0)
        {
            num2 = _FreeList;
            _FreeList = _Next[num2];
            _FreeCount--;
        }
        else
        {
            if (_Count == GetKeysLength())
            {
                assert(0);
                //Resize();
                index = hash % GetBucketsLength();
            }
            num2 = _Count;
            _Count++;
        }

        _HashCodes[num2] = hash;
        _Next[num2] = _Buckets[index];
        _Keys[num2] = key;
        _Values[num2] = value;
        _Buckets[index] = num2;
    }

    void Initialize(int capacity)
    {
        int prime = PrimeHelper_GetPrime(capacity);

        //_Buckets = new int[prime];
        for (int i = 0; i < GetBucketsLength(); i++)
            _Buckets[i] = -1;

        //_Keys = new int[prime];
        //_Values = new string[prime];
        //_HashCodes = new int[prime];
        //_Next = new int[prime];

        _FreeList = -1;
    }

    int FindIndex(int key)
    {
        if (_Buckets != null)
        {
            int hash = GetHashCode(key) & 2147483647;
            for (int i = _Buckets[hash % GetBucketsLength()]; i >= 0; i = _Next[i])
            {
                if (_HashCodes[i] == hash && CompareKeys(_Keys[i], key) == 0)
                    return i;
            }
        }
        return -1;
    }

    bool TryGetValue(int key, string& value)
    {
        int index = FindIndex(key);
        if (index >= 0)
        {
            value = _Values[index];
            return true;
        }
        value = {};
        return false;
    }

    string ValueOrDefault(int key)
    {
        return ValueOrDefault(key, {});
    }

    string ValueOrDefault(int key, string defaultValue)
    {
        //return this[key, defaultValue];
        int index = FindIndex(key);
        if (index >= 0)
            return _Values[index];
        return defaultValue;
    }

    static bool PrimeHelper_IsPrime(int candidate)
    {
        if ((candidate & 1) != 0)
        {
            int num = (int)sqrt((double)candidate);
            for (int i = 3; i <= num; i += 2)
            {
                if (candidate % i == 0)
                {
                    return false;
                }
            }
            return true;
        }
        return candidate == 2;
    }

    static int PrimeHelper_GetPrime(int min)
    {
        assert(min >= 0);

        for (int i = 0; i < ArrayCount(PrimeHelper_Primes); i++)
        {
            int prime = PrimeHelper_Primes[i];
            if (prime >= min)
                return prime;
        }
        for (int i = min | 1; i < 2147483647; i += 2)
        {
            if (PrimeHelper_IsPrime(i) && (i - 1) % 101 != 0)
                return i;
        }
        return min;
    }

    static int PrimeHelper_ExpandPrime(int oldSize)
    {
        int num = 2 * oldSize;
        if (num > 2146435069 && 2146435069 > oldSize)
        {
            return 2146435069;
        }
        return PrimeHelper_GetPrime(num);
    }
};
