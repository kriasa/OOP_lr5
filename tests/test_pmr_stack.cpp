#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "../pmr_stack.h"

struct Point {
    int x, y;
    Point(int a, int b) : x(a), y(b) {}
    bool operator==(const Point& other) const { 
        return x == other.x && y == other.y; 
    }
    friend std::ostream& operator<<(std::ostream& os, const Point& p) {
        return os << "Point(" << p.x << ", " << p.y << ")";
    }
};

class PmrStackTest : public ::testing::Test {

protected:

    FixedBlockVectorResource res_{1024};
    PmrStack<int> stack_{&res_};
};


TEST_F(PmrStackTest, BasicPushPopLogic) {
    ASSERT_TRUE(stack_.empty());
    
    stack_.push(10);
    ASSERT_FALSE(stack_.empty());
    ASSERT_EQ(stack_.top(), 10);

    stack_.push(20);
    ASSERT_EQ(stack_.top(), 20);

    stack_.pop();
    ASSERT_EQ(stack_.top(), 10);
    
    stack_.pop();
    ASSERT_TRUE(stack_.empty());
}


TEST_F(PmrStackTest, IteratorOrder) {
    stack_.push(3);
    stack_.push(2);
    stack_.push(1);

    std::vector<int> expected_order = {1, 2, 3};
    std::vector<int> actual_order;

    for (int val : stack_) {
        actual_order.push_back(val);
    }
    ASSERT_EQ(actual_order, expected_order);
}

TEST(PmrStackComplexTest, StructSupport) {

    FixedBlockVectorResource res(1024);
    PmrStack<Point> stack(&res);
    stack.push(10, 20); 
    ASSERT_EQ(stack.top(), Point(10, 20));
}

TEST(PmrStackResourceTest, MemoryReuse) {
    FixedBlockVectorResource res(64);
    PmrStack<int> stack(&res);

    stack.push(1);
    stack.push(2);
    stack.push(3);
    stack.pop();
    stack.pop();
    ASSERT_NO_THROW(stack.push(4)); 
    ASSERT_NO_THROW(stack.push(5));
    
    ASSERT_EQ(stack.top(), 5);
}