
// Copyright 2010-2014, D. E. Shaw Research.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt )

#include <boost/random/threefry.hpp>
#include <boost/cstdint.hpp>

#include "test_kat.hpp"

using boost::random::threefry;

using boost::random::test::RandomNumberFunctor;
BOOST_CONCEPT_ASSERT((RandomNumberFunctor< boost::random::threefry<2, uint32_t> >));
BOOST_CONCEPT_ASSERT((RandomNumberFunctor< boost::random::threefry<2, uint64_t> >));
BOOST_CONCEPT_ASSERT((RandomNumberFunctor< boost::random::threefry<4, uint32_t> >));
BOOST_CONCEPT_ASSERT((RandomNumberFunctor< boost::random::threefry<4, uint64_t> >));

// The KAT vectors are cut-and-pasted from the kat_vectors file
// in the original Random123 distribution.  The generators that
// produce these known answers have been extensively tested and are
// known to be Crush-Resistant (see Salmon et al, "Parallel Random
// Numbers:  As Easy as 1, 2, 3")
BOOST_AUTO_TEST_CASE(test_kat_threefry2x32)
{
    dokat<threefry<2, uint32_t, 13> > ("00000000 00000000 00000000 00000000 9d1c5ec6 8bd50731  ");
    dokat<threefry<2, uint32_t, 13> > ("ffffffff ffffffff ffffffff ffffffff fd36d048 2d17272c  ");
    dokat<threefry<2, uint32_t, 13> > ("243f6a88 85a308d3 13198a2e 03707344 ba3e4725 f27d669e  ");
    dokat<threefry<2, uint32_t, 20> > ("00000000 00000000 00000000 00000000   6b200159 99ba4efe");
    dokat<threefry<2, uint32_t, 20> > ("ffffffff ffffffff ffffffff ffffffff   1cb996fc bb002be7");
    dokat<threefry<2, uint32_t, 20> > ("243f6a88 85a308d3 13198a2e 03707344   c4923a9c 483df7a0");
}

BOOST_AUTO_TEST_CASE(test_kat_threefry4x32)
{
    dokat<threefry<4, uint32_t, 13> > ("00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 531c7e4f 39491ee5 2c855a92 3d6abf9a  ");
    dokat<threefry<4, uint32_t, 13> > ("ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff c4189358 1c9cc83a d5881c67 6a0a89e0  ");
    dokat<threefry<4, uint32_t, 13> > ("243f6a88 85a308d3 13198a2e 03707344 a4093822 299f31d0 082efa98 ec4e6c89 4aa71d8f 734738c2 431fc6a8 ae6debf1  ");
    dokat<threefry<4, uint32_t, 20> > ("00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000   9c6ca96a e17eae66 fc10ecd4 5256a7d8");
    dokat<threefry<4, uint32_t, 20> > ("ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff   2a881696 57012287 f6c7446e a16a6732");
    dokat<threefry<4, uint32_t, 20> > ("243f6a88 85a308d3 13198a2e 03707344 a4093822 299f31d0 082efa98 ec4e6c89   59cd1dbb b8879579 86b5d00c ac8b6d84");
}

BOOST_AUTO_TEST_CASE(test_kat_threefry2x64)
{
    dokat<threefry<2, uint64_t, 13> > ("0000000000000000 0000000000000000 0000000000000000 0000000000000000 f167b032c3b480bd e91f9fee4b7a6fb5  ");
    dokat<threefry<2, uint64_t, 13> > ("ffffffffffffffff ffffffffffffffff ffffffffffffffff ffffffffffffffff ccdec5c917a874b1 4df53abca26ceb01  ");
    dokat<threefry<2, uint64_t, 13> > ("243f6a8885a308d3 13198a2e03707344 a4093822299f31d0 082efa98ec4e6c89 c3aac71561042993 3fe7ae8801aff316  ");
    dokat<threefry<2, uint64_t, 20> > ("0000000000000000 0000000000000000 0000000000000000 0000000000000000   c2b6e3a8c2c69865 6f81ed42f350084d");
    dokat<threefry<2, uint64_t, 20> > ("ffffffffffffffff ffffffffffffffff ffffffffffffffff ffffffffffffffff   e02cb7c4d95d277a d06633d0893b8b68");
    dokat<threefry<2, uint64_t, 20> > ("243f6a8885a308d3 13198a2e03707344 a4093822299f31d0 082efa98ec4e6c89   263c7d30bb0f0af1 56be8361d3311526");
}

BOOST_AUTO_TEST_CASE(test_kat_threefry4x64)
{
    dokat<threefry<4, uint64_t, 13> > ("0000000000000000 0000000000000000 0000000000000000 0000000000000000 0000000000000000 0000000000000000 0000000000000000 0000000000000000 4071fabee1dc8e05 02ed3113695c9c62 397311b5b89f9d49 e21292c3258024bc  ");
    dokat<threefry<4, uint64_t, 13> > ("ffffffffffffffff ffffffffffffffff ffffffffffffffff ffffffffffffffff ffffffffffffffff ffffffffffffffff ffffffffffffffff ffffffffffffffff 7eaed935479722b5 90994358c429f31c 496381083e07a75b 627ed0d746821121  ");
    dokat<threefry<4, uint64_t, 13> > ("243f6a8885a308d3 13198a2e03707344 a4093822299f31d0 082efa98ec4e6c89 452821e638d01377 be5466cf34e90c6c c0ac29b7c97c50dd 3f84d5b5b5470917 4361288ef9c1900c 8717291521782833 0d19db18c20cf47e a0b41d63ac8581e5  ");
    dokat<threefry<4, uint64_t, 20> > ("0000000000000000 0000000000000000 0000000000000000 0000000000000000 0000000000000000 0000000000000000 0000000000000000 0000000000000000   09218ebde6c85537 55941f5266d86105 4bd25e16282434dc ee29ec846bd2e40b");
    dokat<threefry<4, uint64_t, 20> > (" ffffffffffffffff ffffffffffffffff ffffffffffffffff ffffffffffffffff ffffffffffffffff ffffffffffffffff ffffffffffffffff ffffffffffffffff 29c24097942bba1b 0371bbfb0f6f4e11 3c231ffa33f83a1c cd29113fde32d168 ");
    dokat<threefry<4, uint64_t, 20> > ("243f6a8885a308d3 13198a2e03707344 a4093822299f31d0 082efa98ec4e6c89 452821e638d01377 be5466cf34e90c6c be5466cf34e90c6c c0ac29b7c97c50dd   a7e8fde591651bd9 baafd0c30138319b 84a5c1a729e685b9 901d406ccebc1ba4");
}

