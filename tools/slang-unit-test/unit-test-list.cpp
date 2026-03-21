// unit-test-list.cpp

#include "core/slang-basic.h"
#include "unit-test/slang-unit-test.h"

using namespace Slang;

namespace
{
template<typename T, typename GetValueFunc>
void checkListIsSortedBy(const List<T>& list, const GetValueFunc& getValue)
{
    for (Index i = 1; i < list.getCount(); i++)
        SLANG_CHECK(getValue(list[i - 1]) <= getValue(list[i]));
}

template<typename T>
void checkListIsSorted(const List<T>& list)
{
    checkListIsSortedBy(list, [](const T& value) { return value; });
}
} // namespace

SLANG_UNIT_TEST(list)
{
    // Basic binarySearch and lowerBounds operation.
    {
        List<int> values = {10, 20, 30, 40};

        SLANG_CHECK(values.binarySearch(5) == -1);
        SLANG_CHECK(values.lowerBound(5) == 0);

        SLANG_CHECK(values.binarySearch(10) == 0);
        SLANG_CHECK(values.lowerBound(10) == 0);

        SLANG_CHECK(values.binarySearch(25) == -1);
        SLANG_CHECK(values.lowerBound(25) == 2);

        SLANG_CHECK(values.binarySearch(30) == 2);
        SLANG_CHECK(values.lowerBound(30) == 2);

        SLANG_CHECK(values.binarySearch(50) == -1);
        SLANG_CHECK(values.lowerBound(50) == 4);

        auto insertAndCheck = [&](int value, Index expectedInsertIndex)
        {
            SLANG_CHECK(values.binarySearch(value) == -1);

            Index insertIndex = values.lowerBound(value);
            SLANG_CHECK(insertIndex == expectedInsertIndex);

            values.insert(insertIndex, value);

            checkListIsSorted(values);

            Index foundIndex = values.binarySearch(value);
            SLANG_CHECK(foundIndex >= 0);
            SLANG_CHECK(values[foundIndex] == value);
        };

        insertAndCheck(5, 0);
        insertAndCheck(25, 3);
        insertAndCheck(50, 6);
    }

    // Using custom comparers.
    {
        struct Entry
        {
            int key;
        };

        List<Entry> entries;
        entries.add(Entry{1});
        entries.add(Entry{4});
        entries.add(Entry{9});

        auto comparer = [](const Entry& entry, int key)
        {
            if (entry.key < key)
                return -1;
            if (entry.key > key)
                return 1;
            return 0;
        };

        auto insertAndCheck = [&](int key, Index expectedInsertIndex)
        {
            SLANG_CHECK(entries.binarySearch(key, comparer) == -1);

            Index insertIndex = entries.lowerBound(key, comparer);
            SLANG_CHECK(insertIndex == expectedInsertIndex);

            entries.insert(insertIndex, Entry{key});

            checkListIsSortedBy(entries, [](const Entry& entry) { return entry.key; });

            Index foundIndex = entries.binarySearch(key, comparer);
            SLANG_CHECK(foundIndex >= 0);
            SLANG_CHECK(entries[foundIndex].key == key);
        };

        SLANG_CHECK(entries.binarySearch(0, comparer) == -1);
        SLANG_CHECK(entries.lowerBound(0, comparer) == 0);

        SLANG_CHECK(entries.binarySearch(4, comparer) == 1);
        SLANG_CHECK(entries.lowerBound(4, comparer) == 1);

        SLANG_CHECK(entries.binarySearch(6, comparer) == -1);
        SLANG_CHECK(entries.lowerBound(6, comparer) == 2);

        SLANG_CHECK(entries.binarySearch(10, comparer) == -1);
        SLANG_CHECK(entries.lowerBound(10, comparer) == 3);

        insertAndCheck(0, 0);
        insertAndCheck(6, 3);
        insertAndCheck(10, 5);
    }

    // Empty list
    {
        List<int> values;
        SLANG_CHECK(values.binarySearch(42) == -1);
        SLANG_CHECK(values.lowerBound(42) == 0);
    }

    // Single element
    {
        List<int> values = {10};
        SLANG_CHECK(values.binarySearch(10) >= 0);
        SLANG_CHECK(values.lowerBound(10) == 0);

        SLANG_CHECK(values.binarySearch(5) == -1);
        SLANG_CHECK(values.lowerBound(5) == 0);

        SLANG_CHECK(values.binarySearch(15) == -1);
        SLANG_CHECK(values.lowerBound(15) == 1);

        Index insertIndex = values.lowerBound(5);
        values.insert(insertIndex, 5);
        SLANG_CHECK(values.getCount() == 2);
        checkListIsSorted(values);
        SLANG_CHECK(values[0] == 5);
        SLANG_CHECK(values[1] == 10);
        SLANG_CHECK(values.binarySearch(5) >= 0);
    }

    // Duplicate values in lowerBound.
    {
        List<int> values = {10, 20, 20, 20, 30};

        SLANG_CHECK(values.lowerBound(5) == 0);
        SLANG_CHECK(values.lowerBound(10) == 0);
        SLANG_CHECK(values.lowerBound(19) == 1);
        SLANG_CHECK(values.lowerBound(20) == 1);
        SLANG_CHECK(values.lowerBound(21) == 4);
        SLANG_CHECK(values.lowerBound(30) == 4);
        SLANG_CHECK(values.lowerBound(35) == 5);

        Index insertIndex = values.lowerBound(20);
        values.insert(insertIndex, 20);

        checkListIsSorted(values);
        SLANG_CHECK(values.lowerBound(20) == 1);
        SLANG_CHECK(values[1] == 20);
        SLANG_CHECK(values[2] == 20);
        SLANG_CHECK(values[3] == 20);
        SLANG_CHECK(values[4] == 20);
    }
}
