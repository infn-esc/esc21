#include <ctgmath>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <cstring>

typedef double Float;       //  The floating-point type to use.
typedef std::uint64_t UInt; //  Unsigned integer of same size as Float.


/*  Define a value with only the high bit of a UInt set.  This is also the
    encoding of floating-point -0.
*/
static constexpr UInt HighBit
    = std::numeric_limits<UInt>::max() ^ std::numeric_limits<UInt>::max() >> 1;


//  Return the encoding of a floating-point number by copying its bits.
static UInt EncodingBits(Float x)
{
    UInt result;
    std::memcpy(&result, &x, sizeof result);
    return result;
}


//  Return the encoding of a floating-point number by using math.
static UInt EncodingMath(Float x)
{
    static constexpr int SignificandBits = std::numeric_limits<Float>::digits;
    static constexpr int MinimumExponent = std::numeric_limits<Float>::min_exponent;

    //  Encode the high bit.
    UInt result = std::signbit(x) ? HighBit : 0;

    //  If the value is zero, the remaining bits are zero, so we are done.
    if (x == 0) return result;

    /*  The C library provides a little-known routine to split a floating-point
        number into a significand and an exponent.  Note that this produces a
        normalized significand, not the actual significand encoding.  Notably,
        it brings significands of subnormals up to at least 1/2.  We will
        adjust for that below.  Also, this routine normalizes to [1/2, 1),
        whereas IEEE 754 is usually expressed with [1, 2), but that does not
        bother us.
    */
    int xe;
    Float xf = std::frexp(fabs(x), &xe);

    //  Test whether the number is subnormal.
    if (xe < MinimumExponent)
    {
        /*  For a subnormal value, the exponent encoding is zero, so we only
            have to insert the significand bits.  This scales the significand
            so that its low bit is scaled to the 1 position and then inserts it
            into the encoding.
        */
        result |= (UInt) std::ldexp(xf, xe - MinimumExponent + SignificandBits);
    }
    else
    {
        /*  For a normal value, the significand is encoded without its leading
            bit.  So we subtract .5 to remove that bit and then scale the
            significand so its low bit is scaled to the 1 position.
        */
        result |= (UInt) std::ldexp(xf - .5, SignificandBits);

        /*  The exponent is encoded with a bias of (in C++'s terminology)
            MinimumExponent - 1.  So we subtract that to get the exponent
            encoding and then shift it to the position of the exponent field.
            Then we insert it into the encoding.
        */
        result |= ((UInt) xe - MinimumExponent + 1) << (SignificandBits-1);
    }

    return result;
}


/*  Return the encoding of a floating-point number.  For illustration, we
    get the encoding with two different methods and compare the results.
*/
static UInt Encoding(Float x)
{
    UInt xb = EncodingBits(x);
    UInt xm = EncodingMath(x);

    if (xb != xm)
    {
        std::cerr << "Internal error encoding" << x << ".\n";
        std::cerr << "\tEncodingBits says " << xb << ".\n";
        std::cerr << "\tEncodingMath says " << xm << ".\n";
        std::exit(EXIT_FAILURE);
    }

    return xb;
}


/*  Return the distance from a to b as the number of values representable in
    Float from one to the other.  b must be greater than or equal to a.  0 is
    counted only once.
*/
static UInt Distance(Float a, Float b)
{
    UInt ae = Encoding(a);
    UInt be = Encoding(b);

    /*  For represented values from +0 to infinity, the IEEE 754 binary
        floating-points are in ascending order and are consecutive.  So we can
        simply subtract two encodings to get the number of representable values
        between them (including one endpoint but not the other).

        Unfortunately, the negative numbers are not adjacent and run the other
        direction.  To deal with this, if the number is negative, we transform
        its encoding by subtracting from the encoding of -0.  This gives us a
        consecutive sequence of encodings from the greatest magnitude finite
        negative number to the greatest finite number, in ascending order
        except for wrapping at the maximum UInt value.

        Note that this also maps the encoding of -0 to 0 (the encoding of +0),
        so the two zeroes become one point, so they are counted only once.
    */
    if (HighBit & ae) ae = HighBit - ae;
    if (HighBit & be) be = HighBit - be;

    //  Return the distance between the two transformed encodings.
    return be - ae;
}


static void Try(Float a, Float b)
{
    std::cout << "[" << a << ", " << b << "] contains: \t"
        << Distance(a,b) + 1 << " representable values.\n";
}


int main(void)
{
    if (sizeof(Float) != sizeof(UInt))
    {
        std::cerr << "Error, UInt must be an unsigned integer the same size as Float.\n";
        std::exit(EXIT_FAILURE);
    }

    /*  Prepare some test values:  smallest positive (subnormal) value, largest
        subnormal value, smallest normal value.
    */
    Float S1 = std::numeric_limits<Float>::denorm_min();
    Float N1 = std::numeric_limits<Float>::min();
    Float S2 = N1 - S1;

    Try(   0.0,   0.1);
    Try(   0.1,   0.2);
    Try(   0.2,   0.4);
    Try(   0.4,   0.8);
    Try(   0.8,   1.6);
    Try(   1.6,   3.2);

    /*
    //  Test 0 <= a <= b.
    Try( 0,  0);
    Try( 0, S1);
    Try( 0, S2);
    Try( 0, N1);
    Try( 0, 1./3);
    Try(S1, S1);
    Try(S1, S2);
    Try(S1, N1);
    Try(S1, 1./3);
    Try(S2, S2);
    Try(S2, N1);
    Try(S2, 1./3);
    Try(N1, N1);
    Try(N1, 1./3);

    //  Test a <= b <= 0.
    Try(-0., -0.);
    Try(-S1, -0.);
    Try(-S2, -0.);
    Try(-N1, -0.);
    Try(-1./3, -0.);
    Try(-S1, -S1);
    Try(-S2, -S1);
    Try(-N1, -S1);
    Try(-1./3, -S1);
    Try(-S2, -S2);
    Try(-N1, -S2);
    Try(-1./3, -S2);
    Try(-N1, -N1);
    Try(-1./3, -N1);

    //  Test a <= 0 <= b.
    Try(-0., +0.);
    Try(-0., S1);
    Try(-0., S2);
    Try(-0., N1);
    Try(-0., 1./3);
    Try(-S1, +0.);
    Try(-S1, S1);
    Try(-S1, S2);
    Try(-S1, N1);
    Try(-S1, 1./3);
    Try(-S2, +0.);
    Try(-S2, S1);
    Try(-S2, S2);
    Try(-S2, N1);
    Try(-S2, 1./3);
    Try(-N1, +0.);
    Try(-N1, S1);
    Try(-N1, S2);
    Try(-N1, N1);
    Try(-1./3, 1./3);
    Try(-1./3, +0.);
    Try(-1./3, S1);
    Try(-1./3, S2);
    Try(-1./3, N1);
    Try(-1./3, 1./3);
*/

    return 0;
}
