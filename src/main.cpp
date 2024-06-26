#include <boost/program_options.hpp>
#include "Simulator.h"
#include "util_sim.h"

// void* threading(void* x){
//     int* val = (int*) x;
//     sleep(10);
//     std::cout<<"Hi"<<*val<<std::endl;
//     pthread_exit(NULL);
// }

int main(int argc, char **argv)
{
    namespace po = boost::program_options;
    po::options_description description("Options");
    description.add_options()
    ("help", "produce help message")
    ("sim_qasm", po::value<std::string>()->implicit_value(""), "simulate qasm file string")
    ("seed", po::value<unsigned int>()->implicit_value(1), "seed for random number generator")
    ("print_info", "print simulation statistics such as runtime, memory, etc.")
    ("shots", po::value<unsigned int>()->default_value(1), "the number of outcomes being sampled in \"sampling mode\". " )
    ("r", po::value<unsigned int>()->default_value(32), "integer bit size.")
    ("reorder", po::value<bool>()->default_value(1), "allow variable reordering or not.\n"
                                                             "0: disable reordering.\n"
                                                             "1: enable reordering (default option).")
    ("alloc", po::value<bool>()->default_value(1), "allocate new BDDs when overflow is detected.\n"
                                                    "0: do not allocate new BDDs. This may lead to numerical errors.\n"
                                                    "1: allocate new BDDs (default option).")
    ;
    
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, description), vm);
    po::notify(vm);

    if (vm.count("help") || argc == 1) 
    {
	    std::cout << description << std::endl;
	    return 1;
	}

    struct timeval t1, t2;
    double elapsedTime;

    int shots = vm["shots"].as<unsigned int>(), r = vm["r"].as<unsigned int>();
    bool isReorder = vm["reorder"].as<bool>(), isAlloc = vm["alloc"].as<bool>();

    std::random_device rd;
    unsigned int seed;
    if (vm.count("seed")) 
		seed = vm["seed"].as<unsigned int>();
    else
        seed = rd();

    // start timer
    gettimeofday(&t1, NULL);

    assert(shots > 0);
    Simulator simulator(shots, seed, 1, isReorder, isAlloc);

    if (vm.count("sim_qasm"))
    {
        // read in file into a string
        std::stringstream strStream;
        if (vm["sim_qasm"].as<std::string>() == "")
        {
            strStream << std::cin.rdbuf();
        }
        else
        {
            std::ifstream inFile;
            inFile.open(vm["sim_qasm"].as<std::string>()); //open the input file
            strStream << inFile.rdbuf(); //read the file
        }
        
        std::string inFile_str = strStream.str(); //str holds the content of the file
        simulator.sim_qasm(inFile_str);
    }

    //end timer
    gettimeofday(&t2, NULL);
    elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;
    elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;

    double runtime = elapsedTime / 1000;
    size_t memPeak = getPeakRSS();
    if (vm.count("print_info"))
        simulator.print_info(runtime, memPeak);
    return 0;
}