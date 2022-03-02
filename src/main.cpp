#include <sstream>

#include <seqan3/argument_parser/all.hpp>
#include <seqan3/core/debug_stream.hpp>

#include "compare.h"

uint32_t w_size;
uint64_t shape{};
uint64_t se;

void string_to_methods(std::string name, methods & m)
{
    if (name == "kmer")
        m = kmer;
    else if (name == "minimiser")
        m = minimiser;
    else if (name == "modmer")
        m = modmers;
    else if (name == "strobemer")
        m = strobemer;
    else if (name == "syncmer")
        m = syncmer;
};

void all_arguments(seqan3::argument_parser & parser, range_arguments & args)
{
    parser.add_option(args.path_out, 'o', "out",
                      "Directory, where output files should be saved.");
    parser.add_option(args.k_size, 'k', "kmer-size", "Define kmer size.");
}

void read_range_arguments_strobemers(seqan3::argument_parser & parser, range_arguments & args)
{
    parser.add_option(args.w_min, '\0', "w-min", "Define w-min for strobemers.");
    parser.add_option(args.w_max, '\0', "w-max", "Define w-ax for strobemers.");
    parser.add_option(args.order, '\0', "order", "Define order for strobemers.", seqan3::option_spec::standard, seqan3::arithmetic_range_validator{2,3});
    parser.add_flag(args.rand, '\0', "randstrobemers", "If randstrobemers should be calculated.");
    parser.add_flag(args.hybrid, '\0', "hybrid", "If hybridstrobemers should be calculated.");
    parser.add_flag(args.minstrobers, '\0', "minstrobers", "If minstrobemers should be calculated.");
}

void read_range_arguments_syncmers(seqan3::argument_parser & parser, range_arguments & args)
{
    parser.add_flag(args.closed, '\0', "closed", "If closed syncmers should be calculated.");
}

void read_range_arguments_minimiser(seqan3::argument_parser & parser, range_arguments & args)
{
    parser.add_option(w_size, 'w', "window", "Define window size. Default: 60.");
    parser.add_option(shape, '\0', "shape", "Define a shape by the decimal of a bitvector, where 0 symbolizes a "
                                           "position to be ignored, 1 a position considered. Default: ungapped.");
    parser.add_option(se, '\0', "seed", "Define seed.");
}

void parsing(range_arguments & args)
{
    args.w_size = seqan3::window_size{w_size};
    if (shape == 0)
        args.shape = seqan3::ungapped{args.k_size};
    else
        args.shape = seqan3::bin_literal{shape};
    args.seed_se = seqan3::seed{adjust_seed(args.k_size, se)};
}

int accuracy(seqan3::argument_parser & parser)
{
    accuracy_arguments args{};
    parser.info.short_description = "Counts the true positive, false positive, "
                                    "true negatives and false negatives of a "
                                    "sequence file given the ground truth by a "
                                    "solution file.";
    parser.add_positional_option(args.input_file, "Either one input file containg the"
                                             " ibf with the file extension"
                                             "'.ibf' or multiple preprocessed "
                                             "binary files ending with '.out'.");
    all_arguments(parser, args);
    std::string method{};
    parser.add_option(method, '\0', "method", "Pick your method.",
                      seqan3::option_spec::required,
                      seqan3::value_list_validator{"kmer", "minimiser", "modmer", "syncmer"});
    parser.add_option(args.search_file, '\0', "search-file", "A sequence files with sequences to search for.",
                      seqan3::option_spec::required);
    parser.add_option(args.solution_file, '\0', "solution-file", "A file giving the correct files a sequence should be find in.",
                      seqan3::option_spec::required);
    parser.add_option(args.ibfsize, '\0', "ibfsize", "The size of the ibf.",
                      seqan3::option_spec::advanced);
    parser.add_option(args.number_hashes, '\0', "number-hashes",
                      "The number of hashes to use.",
                      seqan3::option_spec::advanced);
    parser.add_option(args.threshold, '\0', "threshold",
                      "The threshold to use for the search.",
                      seqan3::option_spec::advanced);

    read_range_arguments_minimiser(parser, args);
    read_range_arguments_syncmers(parser, args);

    try
    {
        parser.parse();
        parsing(args);
    }
    catch (seqan3::argument_parser_error const & ext)                     // catch user errors
    {
        seqan3::debug_stream << "Error. Incorrect command line input for accuracy. " << ext.what() << "\n";
        return -1;
    }

    string_to_methods(method, args.name);
    do_accuracy(args);

    return 0;
}

int counts(seqan3::argument_parser & parser)
{
    range_arguments args{};
    std::vector<std::filesystem::path> sequence_files{};
    parser.info.short_description = "Counts the number of submers in the given "
                                    "sequence files.";
    parser.add_positional_option(sequence_files,
                                 "Please provide at least one sequence file.");
    all_arguments(parser, args);
    std::string method{};
    parser.add_option(method, '\0', "method", "Pick your method.",
                      seqan3::option_spec::required, seqan3::value_list_validator{"kmer", "minimiser", "modmer", "strobemer", "syncmer"});

    read_range_arguments_minimiser(parser, args);
    read_range_arguments_strobemers(parser, args);
    read_range_arguments_syncmers(parser, args);

    try
    {
        parser.parse();
        parsing(args);
    }
    catch (seqan3::argument_parser_error const & ext)                     // catch user errors
    {
        seqan3::debug_stream << "Error. Incorrect command line input for counts. " << ext.what() << "\n";
        return -1;
    }

    string_to_methods(method, args.name);
    do_counts(sequence_files, args);

    return 0;
}

int coverage(seqan3::argument_parser & parser)
{
    range_arguments args{};
    std::filesystem::path sequence_file{};
    parser.info.short_description = "Estimates the coverage of the different methods.";
    parser.add_positional_option(sequence_file, "Please provide one sequence file.");
    all_arguments(parser, args);
    std::string method{};
    parser.add_option(method, '\0', "method", "Pick your method.",
                      seqan3::option_spec::required,
                      seqan3::value_list_validator{"kmer", "minimiser", "modmer"});

    read_range_arguments_minimiser(parser, args);

    try
    {
        parser.parse();
        parsing(args);
    }
    catch (seqan3::argument_parser_error const & ext)                     // catch user errors
    {
        seqan3::debug_stream << "Error. Incorrect command line input for coverage. " << ext.what() << "\n";
        return -1;
    }

    string_to_methods(method, args.name);
    do_coverage(sequence_file, args);

    return 0;
}

int speed(seqan3::argument_parser & parser)
{
    range_arguments args{};
    std::vector<std::filesystem::path> sequence_files{};
    parser.info.short_description = "Estimates the speed of a method for the given sequence files.";
    parser.add_positional_option(sequence_files,
                                 "Please provide at least one sequence file.");
    all_arguments(parser, args);
    std::string method{};
    parser.add_option(method, '\0', "method", "Pick your method.",
                      seqan3::option_spec::required, seqan3::value_list_validator{"kmer", "minimiser", "modmer", "strobemer"});

    read_range_arguments_minimiser(parser, args);
    read_range_arguments_strobemers(parser, args);

    try
    {
        parser.parse();
        parsing(args);
    }
    catch (seqan3::argument_parser_error const & ext)                     // catch user errors
    {
        seqan3::debug_stream << "Error. Incorrect command line input for speed. " << ext.what() << "\n";
        return -1;
    }

    string_to_methods(method, args.name);
    do_speed(sequence_files, args);

    return 0;
}

int main(int argc, char ** argv)
{
    seqan3::argument_parser top_level_parser{"minions", argc, argv,
                                             seqan3::update_notifications::on,
                                             {"accuracy", "counts", "coverage", "speed"}};

    // Parser
    top_level_parser.info.author = "Mitra Darvish"; // give parser some infos
    top_level_parser.info.version = "0.1.0";

    try
    {
         top_level_parser.parse();  // trigger command line parsing
    }
    catch (seqan3::argument_parser_error const & ext) // catch user errors
    {
        seqan3::debug_stream << "Parsing error. " << ext.what() << "\n"; // give error message
        return -1;
    }

    seqan3::argument_parser & sub_parser = top_level_parser.get_sub_parser(); // hold a reference to the sub_parser

    if (sub_parser.info.app_name == std::string_view{"minions-accuracy"})
        accuracy(sub_parser);
    else if (sub_parser.info.app_name == std::string_view{"minions-counts"})
        counts(sub_parser);
    else if (sub_parser.info.app_name == std::string_view{"minions-coverage"})
        coverage(sub_parser);
    else if (sub_parser.info.app_name == std::string_view{"minions-speed"})
        speed(sub_parser);

    return 0;
}
