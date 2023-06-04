#include <iostream>

#define BOOST_TEST_MODULE boost_tests
// Following macroes could be omitted if no need to define yourself main().
#define BOOST_TEST_NO_MAIN
#define BOOST_TEST_ALTERNATIVE_INIT_API
#include <boost/test/included/unit_test.hpp>

class Point3D {
	int __x, __y, __z;

public:
	constexpr explicit Point3D(int x = 0, int y = 0, int z = 0): __x(x), __y(y), __z(z) {}

	int x() const { return __x; }
	Point3D & x(int x) { __x = x; return *this; }

	int y() const { return __y; }
	Point3D & y(int y) { __y = y; return *this; }

	int z() const { return __z; }
	Point3D & z(int z) { __z = z; return *this; }

	bool operator ==(Point3D const &other) const
	{
		return __x == other.__x && __y == other.__y && __z == other.__z;
	}

	bool operator <(Point3D const &other) const
	{
		return __x < other.__x || __y < other.__y || __z < other.__z;
	}

	bool operator >(Point3D const &other) const
	{
		return other < *this;
	}

	static Point3D const & origin()
	{
		static Point3D __origin;

		return __origin;
	}

	Point3D & offset(int x, int y, int z)
	{
		__x += x;
		__y += y;
		__z += z;

		return *this;
	}
};

std::ostream & operator <<(std::ostream & os, Point3D const &obj)
{
	os << '(' << obj.x() << ',' << obj.y() << ',' << obj.z() << ')';
	return os;
}

BOOST_AUTO_TEST_SUITE(demo_operations)
BOOST_AUTO_TEST_SUITE(demo_constructors)

BOOST_AUTO_TEST_CASE(demo_ctor,
		*boost::unit_test::description("Non-default constructor test.")
		*boost::unit_test::label("op_ctor"))
{
	Point3D vBeiJing(13, 14, 15);

	BOOST_TEST(vBeiJing.x() == 13);
	BOOST_TEST(vBeiJing.y() == 14);
	BOOST_TEST(vBeiJing.z() == 15);
}

BOOST_AUTO_TEST_CASE(demo_origin,
		*boost::unit_test::description("static member of class Point3D")
		*boost::unit_test::label("op_origin"))
{
	Point3D const & vOrigin = Point3D::origin();

	BOOST_TEST(vOrigin.x() == 0);
	BOOST_TEST(vOrigin.y() == 0);
	BOOST_TEST(vOrigin.z() == 0);
}

BOOST_AUTO_TEST_SUITE_END()  // demo_constructors

BOOST_AUTO_TEST_SUITE(demo_comparisons)

BOOST_AUTO_TEST_CASE(demo_equal)
{
	Point3D vBeiJing(13, 14, 15);
	Point3D vPeking(13, 14, 15);

	BOOST_TEST(vBeiJing == vPeking);
}

BOOST_AUTO_TEST_CASE(demo_not_equal,
		*boost::unit_test::description("inequality of Point3D objects")
		*boost::unit_test::label("op_ne")
		*boost::unit_test::depends_on("demo_operations/demo_comparisons/demo_equal"))
{
	Point3D vBeiJing(13, 14, 15);
	Point3D vTianJing(10, 14, 15);

	BOOST_TEST(vBeiJing != vTianJing);
}

BOOST_AUTO_TEST_CASE(demo_less,
		*boost::unit_test::description("less comparison of Point3D objects")
		*boost::unit_test::label("op_lt"))

{
	Point3D vBeiJing(13, 14, 15);
	Point3D vTianJing(10, 14, 15);

	BOOST_TEST(vTianJing < vBeiJing);
}

BOOST_AUTO_TEST_CASE(demo_greater,
		*boost::unit_test::description("greater comparison of Point3D objects")
		*boost::unit_test::label("op_gt")
		*boost::unit_test::depends_on("demo_operations/demo_comparisons/demo_less"))
{
	Point3D vBeiJing(13, 14, 15);
	Point3D vTianJing(10, 14, 15);

	BOOST_TEST(vBeiJing > vTianJing);
}

BOOST_AUTO_TEST_CASE(demo_offset)
{
	Point3D vHome(9, 9, 9);

	vHome.offset(-3, -3, -3);

	BOOST_TEST(vHome.x() == 6);
	BOOST_TEST(vHome.y() == 6);
	BOOST_TEST(vHome.z() == 6);
}

BOOST_AUTO_TEST_SUITE_END()  // demo_comparisons

BOOST_AUTO_TEST_SUITE_END()  // demo_operations

int main(int argc, char *argv[])
{
	std::cout << "This is a demo of boost unit test framework." << std::endl;
	return boost::unit_test::unit_test_main(init_unit_test, argc, argv);
}
