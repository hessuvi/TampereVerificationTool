/*
The contents of this file are subject to the NOKOS License Version 1.0
(the "License"); you may not use this file except in compliance with the
License.

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF  ANY KIND, either express or implied. See the License
for the specific language governing rights and limitations under the License.

The Original Software is
TVT-tools.

Copyright © 2001 Nokia and others. All Rights Reserved.

Contributor(s): Juha Nieminen.
*/

// An implementation of the non-standard drand48() function so that TVT
// would not depend on it.

class Int48
{
 public:
    inline Int48(): lsw_(0x330E16), mw_(0), msw_(0) {}

    inline void add(unsigned lsw)
    {
        lsw_ += lsw;
        mw_  += lsw_>>16;
        msw_ += mw_>>16;
        lsw_ &= 0xFFFF; mw_ &= 0xFFFF; msw_ &= 0xFFFF;
    }

    inline void multiply(unsigned msw, unsigned mw, unsigned lsw)
    {
        unsigned res_lsw = lsw*lsw_;
        unsigned res_mw  = lsw*mw_;
        unsigned res_msw = lsw*msw_ + (res_mw>>16);
        res_mw = (res_mw&0xFFFF)+(res_lsw>>16);
        res_msw += res_mw>>16;
        res_lsw &= 0xFFFF; res_mw &= 0xFFFF;

        unsigned res_lsw2 = mw*lsw_;
        unsigned res_mw2  = mw*mw_ + (res_lsw2>>16);
        res_lsw2 &= 0xFFFF;

        unsigned res_lsw3 = msw*lsw_;

        lsw_ = res_lsw;
        mw_ = res_mw+res_lsw2;
        msw_ = res_msw+res_mw2+res_lsw3+(mw_>>16);
        mw_ &= 0xFFFF; msw_ &= 0xFFFF;
    }

    inline double toDouble()
    {
        return (((msw_<<16)|mw_)*65536.0+lsw_)/281474976710655.0;
    }

    /*
    void print()
    {
        printf("%04X%04X%04X", msw_, mw_, lsw_);
    }
    */

 private:
    unsigned lsw_, mw_, msw_;
};

static Int48 CurrX;

double DRand48()
{
    CurrX.multiply(0x5DE, 0xECE6, 0x6D16);
    CurrX.add(0xB16);
    return CurrX.toDouble();
}
