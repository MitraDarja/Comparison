#include <chrono>

#include <index.hpp>
#include <seqan3/alphabet/adaptation/char.hpp>
#include <seqan3/alphabet/nucleotide/dna5.hpp>
#include <seqan3/core/detail/empty_type.hpp>

#include "compare.h"

/*! \brief Function, comparing the methods.
 *  \param sequence_files A vector of sequence files.
 *  \param input_view View that should be tested.
 *  \param method_name Name of the tested method.
 *  \param args The arguments about the view to be used, needed for strobemers.
 */
template <typename urng_t, int strobemers = 0>
void compare(std::vector<std::filesystem::path> sequence_files, urng_t input_view, std::string method_name, range_arguments & args)
{
    std::vector<int> speed_results{};
    std::ofstream outfile;
    for (int i = 0; i < sequence_files.size(); ++i)
    {
        seqan3::sequence_file_input<my_traits, seqan3::fields<seqan3::field::seq>> fin{sequence_files[i]};
        robin_hood::unordered_node_map<uint64_t, uint16_t> hash_table{};
        auto start = std::chrono::high_resolution_clock::now();
        if constexpr (strobemers == 1)
        {
            for (auto & [seq] : fin)
            {
                std::string seq2 = seq | std::views::transform([](seqan3::dna4 chr){ return chr.to_char(); }) | seqan3::views::to<std::string>;
                typedef std::vector< std::tuple<uint64_t, unsigned int, unsigned int, unsigned int, unsigned int>> strobes_vector;
                strobes_vector randstrobes3 = seq_to_randstrobes2(args.order, args.k_size, args.w_min, args.w_max, seq2, 0);
                for (auto & t : randstrobes3) // iterate over the strobemer tuples
                    hash_table[std::get<0>(t)] = std::min<uint16_t>(65534u, hash_table[std::get<0>(t)] + 1);
            }
        }
        else if constexpr (strobemers == 2)
        {
            for (auto & [seq] : fin)
            {
                std::string seq2 = seq | std::views::transform([](seqan3::dna4 chr){ return chr.to_char(); }) | seqan3::views::to<std::string>;
                typedef std::vector< std::tuple<uint64_t, unsigned int, unsigned int, unsigned int, unsigned int>> strobes_vector;
                strobes_vector randstrobes3 = seq_to_randstrobes3(args.order, args.k_size, args.w_min, args.w_max, seq2, 0);
                for (auto & t : randstrobes3) // iterate over the strobemer tuples
                    hash_table[std::get<0>(t)] = std::min<uint16_t>(65534u, hash_table[std::get<0>(t)] + 1);
            }
        }
        else if constexpr (strobemers == 3)
        {
            for (auto & [seq] : fin)
            {
                std::string seq2 = seq | std::views::transform([](seqan3::dna4 chr){ return chr.to_char(); }) | seqan3::views::to<std::string>;
                typedef std::vector< std::tuple<uint64_t, unsigned int, unsigned int, unsigned int, unsigned int>> strobes_vector;
                strobes_vector randstrobes3 = seq_to_hybridstrobes2(args.order, args.k_size, args.w_min, args.w_max, seq2, 0);
                for (auto & t : randstrobes3) // iterate over the strobemer tuples
                    hash_table[std::get<0>(t)] = std::min<uint16_t>(65534u, hash_table[std::get<0>(t)] + 1);
            }
        }
        else if constexpr (strobemers == 4)
        {
            for (auto & [seq] : fin)
            {
                std::string seq2 = seq | std::views::transform([](seqan3::dna4 chr){ return chr.to_char(); }) | seqan3::views::to<std::string>;
                typedef std::vector< std::tuple<uint64_t, unsigned int, unsigned int, unsigned int, unsigned int>> strobes_vector;
                strobes_vector randstrobes3 = seq_to_minstrobes2(args.order, args.k_size, args.w_min, args.w_max, seq2, 0);
                for (auto & t : randstrobes3) // iterate over the strobemer tuples
                    hash_table[std::get<0>(t)] = std::min<uint16_t>(65534u, hash_table[std::get<0>(t)] + 1);
            }
        }
        else
        {
            for (auto & [seq] : fin)
            {
                for (auto && hash : seq | input_view)
                    hash_table[hash] = std::min<uint16_t>(65534u, hash_table[hash] + 1);
            }
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        speed_results.push_back(duration.count());

        // Store representative k-mers
        outfile.open(std::string{args.path_out} + method_name + "_"+ std::string{sequence_files[i].stem()} + ".out", std::ios::binary);
        for (auto && hash : hash_table)
        {
            outfile.write(reinterpret_cast<const char*>(&hash.first), sizeof(hash.first));
            outfile.write(reinterpret_cast<const char*>(&hash.second), sizeof(hash.second));
        }
        outfile.close();
    }

    double sum = std::accumulate(speed_results.begin(), speed_results.end(), 0.0);
    double mean = sum / speed_results.size();
    std::vector<double> diff(speed_results.size());
    std::transform(speed_results.begin(), speed_results.end(), diff.begin(), [mean](double x) { return x - mean; });
    double sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
    double stdev = std::sqrt(sq_sum / speed_results.size());
    std::cout << method_name << "\t" << *std::min_element(speed_results.begin(), speed_results.end()) << "\t" << mean << "\t" << stdev << "\t" << *std::max_element(speed_results.begin(), speed_results.end()) << "\n";
}

void do_comparison(std::vector<std::filesystem::path> sequence_files, range_arguments & args)
{
    switch(args.name)
    {
        case kmer: compare(sequence_files, seqan3::views::kmer_hash(seqan3::ungapped{args.k_size}), "kmer_hash_"+std::to_string(args.k_size), args);
                   break;
        case minimiser: compare(sequence_files, seqan3::views::minimiser_hash(args.shape,
                                args.w_size), "minimiser_hash_" + std::to_string(args.k_size) + "_" + std::to_string(args.w_size.get()), args);
                        break;
        case strobemer: std::ranges::empty_view<seqan3::detail::empty_type> empty{};
                        if (args.rand & (args.order == 2))
                            compare<std::ranges::empty_view<seqan3::detail::empty_type>, 1>(sequence_files, empty,
                                "randstrobemers_" + std::to_string(args.k_size) + "_" + std::to_string(args.order) + "_" +  std::to_string(args.w_min) + "_" +  std::to_string(args.w_max), args);
                        else if (args.rand & (args.order == 3))
                            compare<std::ranges::empty_view<seqan3::detail::empty_type>, 2>(sequence_files, empty,
                                "randstrobemers_" + std::to_string(args.k_size) + "_" + std::to_string(args.order) + "_" +  std::to_string(args.w_min) + "_" +  std::to_string(args.w_max), args);
                        else if (args.hybrid)
                            compare<std::ranges::empty_view<seqan3::detail::empty_type>, 3>(sequence_files, empty,
                                "hybridstrobemers_" + std::to_string(args.k_size) + "_" + std::to_string(args.order) + "_" +  std::to_string(args.w_min) + "_" +  std::to_string(args.w_max), args);
                        else if (args.minstrobers)
                            compare<std::ranges::empty_view<seqan3::detail::empty_type>, 4>(sequence_files, empty,
                                "minstrobemers_" + std::to_string(args.k_size) + "_" + std::to_string(args.order) + "_" +  std::to_string(args.w_min) + "_" +  std::to_string(args.w_max), args);
    }
}
