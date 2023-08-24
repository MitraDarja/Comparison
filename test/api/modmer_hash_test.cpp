#include <forward_list>
#include <list>
#include <type_traits>

#include <seqan3/alphabet/container/bitpacked_sequence.hpp>
#include <seqan3/alphabet/detail/debug_stream_alphabet.hpp>
#include <seqan3/alphabet/nucleotide/dna4.hpp>
#include <seqan3/alphabet/views/complement.hpp>
#include <seqan3/io/views/detail/take_until_view.hpp>
#include <seqan3/search/views/kmer_hash.hpp>

#include <seqan3/test/expect_range_eq.hpp>

#include <gtest/gtest.h>

#include "../../lib/seqan3/test/unit/range/iterator_test_template.hpp"

#include "modmer_hash.hpp"

using seqan3::operator""_dna4;
using seqan3::operator""_shape;
using result_t = std::vector<size_t>;

using iterator_type = std::ranges::iterator_t<decltype(std::declval<seqan3::dna4_vector&>()
                                                       | modmer_hash(seqan3::ungapped{4},
                                                                     2,
                                                                     seqan3::seed{0}))>;

static constexpr seqan3::shape ungapped_shape = seqan3::ungapped{4};
static constexpr seqan3::shape gapped_shape = 0b1001_shape;
static constexpr auto ungapped_view = modmer_hash(ungapped_shape, 2, seqan3::seed{0});
static constexpr auto ungapped_3_view = modmer_hash(ungapped_shape, 3, seqan3::seed{0});
static constexpr auto gapped_view = modmer_hash(gapped_shape, 2, seqan3::seed{0});

template <>
struct iterator_fixture<iterator_type> : public ::testing::Test
{
    using iterator_tag = std::forward_iterator_tag;
    static constexpr bool const_iterable = false;

    seqan3::dna4_vector text{"ACGGCGACGTTTAG"_dna4};
    result_t expected_range{26, 152, 6, 192, 112};

    using test_range_t = decltype(text | ungapped_view);
    test_range_t test_range = text | ungapped_view;
};

using test_type = ::testing::Types<iterator_type>;
INSTANTIATE_TYPED_TEST_SUITE_P(iterator_fixture, iterator_fixture, test_type, );

template <typename T>
class modmer_hash_view_properties_test: public ::testing::Test { };

using underlying_range_types = ::testing::Types<std::vector<seqan3::dna4>,
                                                std::vector<seqan3::dna4> const,
                                                seqan3::bitpacked_sequence<seqan3::dna4>,
                                                seqan3::bitpacked_sequence<seqan3::dna4> const,
                                                std::list<seqan3::dna4>,
                                                std::list<seqan3::dna4> const>;

TYPED_TEST_SUITE(modmer_hash_view_properties_test, underlying_range_types, );

class modmer_hash_test : public ::testing::Test
{
protected:
    std::vector<seqan3::dna4> text1{"AAAAAA"_dna4};
    result_t result1{0,0,0}; // Same result for ungapped and gapped

    std::vector<seqan3::dna4> too_short_text{"AC"_dna4};

    // ACGG CGGC, GGCG, GCGA, CGAC, GACG, ACGT, CGTT, GTTT, TTTA, TTAG
    // CCGT GCCG  CGCC  TCGC  GTCG  CGTC  ACGT  AACG  AAAC  TAAA  CTAA
    // ACGG CGGC cgcc GCGA CGAC cgtc ACGT aacg aaac taaa ctaa
    std::vector<seqan3::dna4> text3{"ACGGCGACGTTTAG"_dna4};
    result_t result3_ungapped{26, 152, 6, 192, 112};  // ACGG, GCGA, aacg, taaa, ctaa
    result_t result3_gapped{2, 8, 2, 12, 4};       // A--G, G--A, a--g, t--a, c--a - "-" for gap
    result_t result3_ungapped_3{105,27,6, 192};
    result_t result3_ungapped_stopped{26, 152};
    result_t result3_gapped_stopped{2, 8};
    result_t result3_ungapped_start{6, 192, 112};
    result_t result3_gapped_start{2, 12, 4};
};

template <typename adaptor_t>
void compare_types(adaptor_t v)
{
    EXPECT_TRUE(std::ranges::input_range<decltype(v)>);
    EXPECT_TRUE(std::ranges::forward_range<decltype(v)>);
    EXPECT_FALSE(std::ranges::bidirectional_range<decltype(v)>);
    EXPECT_FALSE(std::ranges::random_access_range<decltype(v)>);
    EXPECT_TRUE(std::ranges::view<decltype(v)>);
    EXPECT_FALSE(std::ranges::sized_range<decltype(v)>);
    EXPECT_FALSE(std::ranges::common_range<decltype(v)>);
    EXPECT_TRUE(seqan3::const_iterable_range<decltype(v)>);
    EXPECT_FALSE((std::ranges::output_range<decltype(v), size_t>));
}

TYPED_TEST(modmer_hash_view_properties_test, different_input_ranges)
{
    TypeParam text{'A'_dna4, 'C'_dna4, 'G'_dna4, 'T'_dna4, 'C'_dna4, 'G'_dna4, 'A'_dna4, 'C'_dna4, 'G'_dna4, 'T'_dna4,
                'T'_dna4, 'T'_dna4, 'A'_dna4, 'G'_dna4}; // ACGTCGACGTTTAG
    result_t ungapped{216, 6, 192, 112}; // TCGA, aacg, taaa, ctaa
    result_t gapped{12, 2, 12, 4}; // T--A, a--g, t--a, c--a - "-" for gap
    EXPECT_RANGE_EQ(ungapped, text | ungapped_view);
    EXPECT_RANGE_EQ(gapped, text | gapped_view);
}

TEST_F(modmer_hash_test, ungapped)
{
    EXPECT_RANGE_EQ(result1, text1 | ungapped_view);
    EXPECT_TRUE(std::ranges::empty(too_short_text | ungapped_view));
    EXPECT_RANGE_EQ(result3_ungapped, (text3 | ungapped_view));
    EXPECT_NO_THROW(text1 | modmer_hash(ungapped_shape, 2));
    EXPECT_RANGE_EQ(result3_ungapped_3, text3 | ungapped_3_view);
}

TEST_F(modmer_hash_test, gapped)
{
    EXPECT_RANGE_EQ(result1, text1 | gapped_view);
    EXPECT_TRUE(std::ranges::empty(too_short_text | gapped_view));
    EXPECT_RANGE_EQ(result3_gapped, text3 | gapped_view);
    EXPECT_NO_THROW(text1 | modmer_hash(gapped_shape, 2));
}

TEST_F(modmer_hash_test, combinability)
{
    auto stop_at_t = std::views::take_while([] (seqan3::dna4 const x) { return x != 'T'_dna4; });
    EXPECT_RANGE_EQ(result3_ungapped_stopped, text3 | stop_at_t | ungapped_view);
    EXPECT_RANGE_EQ(result3_gapped_stopped, text3 | stop_at_t | gapped_view);

    auto start_at_a = std::views::drop(6);
    EXPECT_RANGE_EQ(result3_ungapped_start, text3 | start_at_a | ungapped_view);
    EXPECT_RANGE_EQ(result3_gapped_start, text3 | start_at_a | gapped_view);
}
