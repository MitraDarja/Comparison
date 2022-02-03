#include <gtest/gtest.h>

#include <seqan3/test/expect_range_eq.hpp>

#include "compare.h"

TEST(minions, small_example)
{
    range_arguments args{};
    args.name = kmer;
    args.k_size = 19;
    args.shape = seqan3::ungapped{19};
    std::string expected{"kmer_hash_19         	159493	159493	0	159493\n"};
    args.path_out = std::filesystem::temp_directory_path();
    do_comparison({DATADIR"example1.fasta"}, args);

    std::ifstream infile;
    std::string line;
    infile.open(std::string{args.path_out} + std::string{args.path_out}  + "kmer_hash_19_speed_compression.out");
    if(infile.is_open())
    {
        while(std::getline(infile, line))
        {
            std::istringstream iss(line);
            std::vector<std::string> results((std::istream_iterator<std::string>(iss)),
                                     std::istream_iterator<std::string>());
            EXPECT_EQ(expected.substr(0,12), results[0]);
            EXPECT_EQ("0", results[3]);
        }
    }
    infile.close();
}

TEST(minions, accuracy_binary_file)
{
    accuracy_arguments args{};
    args.name = minimiser;
    args.k_size = 19;
    args.w_size = seqan3::window_size{19};
    args.shape = seqan3::ungapped{19};
    args.seed_se = seqan3::seed{adjust_seed(args.k_size)};
    args.input_file = {DATADIR"minimiser_hash_19_19_example1.out"};
    args.ibfsize = 1000000;
    args.path_out = std::filesystem::path{std::string{std::filesystem::temp_directory_path()} + "/bin_"};
    args.search_file = DATADIR"search.fasta";
    do_accuracy(args);

    seqan3::interleaved_bloom_filter ibf{};
    load_ibf(ibf, std::string{args.path_out} + "minimiser_hash_19_19.ibf");
    auto agent = ibf.membership_agent();

    // Check if ibf was created correctly
    std::vector<bool> expected_result(1, 1);
    auto & res = agent.bulk_contains(39030638997);
    EXPECT_RANGE_EQ(expected_result,  res);

    std::vector<std::string> expected{"test	0,", "test2	0,"};
    int i{0};
    std::ifstream infile{std::string{args.path_out} + "minimiser_hash_19_19_" + std::string{args.search_file.stem()} + ".search_out"};
    std::string line;
    while (std::getline(infile, line))
    {
        std::istringstream iss(line);
        EXPECT_EQ(expected[i], line);
        i++;
    }
    std::filesystem::remove(std::string{args.path_out} + "minimiser_hash_19_19.ibf");
    std::filesystem::remove(std::string{args.path_out} + "minimiser_hash_19_19_" + std::string{args.search_file.stem()} + ".search_out");
}

TEST(minions, accuracy_existing_ibf)
{
    accuracy_arguments args{};
    args.name = minimiser;
    args.k_size = 19;
    args.w_size = seqan3::window_size{19};
    args.shape = seqan3::ungapped{19};
    args.seed_se = seqan3::seed{adjust_seed(args.k_size)};
    args.input_file = {DATADIR"example.ibf"};
    args.path_out = std::filesystem::path{std::string{std::filesystem::temp_directory_path()} + "/"};
    args.search_file = DATADIR"search.fasta";
    args.threshold = 0.5;
    do_accuracy(args);

    std::vector<std::string> expected{"test	0,", "test2	0,"};
    int i{0};
    std::ifstream infile{std::string{args.path_out} + "minimiser_hash_19_19_" + std::string{args.search_file.stem()} + ".search_out"};
    std::string line;
    while (std::getline(infile, line))
    {
        std::istringstream iss(line);
        EXPECT_EQ(expected[i], line);
        i++;
    }
    std::filesystem::remove(std::string{args.path_out} + "minimiser_hash_19_19.ibf");
    std::filesystem::remove(std::string{args.path_out} + "minimiser_hash_19_19_" + std::string{args.search_file.stem()} + ".search_out");
}
