/**
 * Introduction to Operating Systems - Spring 2025
 * Assignment 10 - Second Chance Page Replacement Algorithm Simulation
 *
 * This program simulates the second chance page replacement algorithm
 * in a 32-bit computer system.
 */

#include <iostream>
#include <vector>
#include <random>
#include <cstdlib>
#include <fstream>
#include <iomanip>

// Function prototypes
std::vector<int> PageTraceGenerator(int n, int p);
int Simulate(const std::vector<int> &page_trace, int f);
int FindVictim(std::vector<unsigned long> &page_table);

/**
 * Main routine that accepts two parameters n and p
 * n = length of page trace (≥16)
 * p = number of pages (≥8)
 */
int main(int argc, char *argv[])
{
    // Check if correct number of arguments are provided
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <n> <p>" << std::endl;
        std::cerr << "  n = length of page trace (≥16)" << std::endl;
        std::cerr << "  p = number of pages (≥8)" << std::endl;
        return 1;
    }

    // Parse command line arguments
    int n = std::atoi(argv[1]);
    int p = std::atoi(argv[2]);

    // Validate arguments
    if (n < 16)
    {
        std::cerr << "Error: n must be ≥16" << std::endl;
        return 1;
    }
    if (p < 8)
    {
        std::cerr << "Error: p must be ≥8" << std::endl;
        return 1;
    }

    std::cout << "Simulating Second Chance Page Replacement Algorithm" << std::endl;
    std::cout << "  Page trace length (n): " << n << std::endl;
    std::cout << "  Number of pages (p): " << p << std::endl;

    // Generate a random page trace
    std::vector<int> page_trace = PageTraceGenerator(n, p);

    // Open a file to save results
    std::ofstream results_file("page_fault_results.csv");
    if (!results_file.is_open())
    {
        std::cerr << "Error opening output file" << std::endl;
        return 1;
    }
    results_file << "Frames,PageFaults" << std::endl;

    // Run simulation with varying number of frames (4 to p)
    std::cout << "\nRunning simulations with different frame counts:" << std::endl;
    std::cout << "---------------------------------------------" << std::endl;
    std::cout << "Frames | Page Faults" << std::endl;
    std::cout << "-------+------------" << std::endl;

    for (int f = 4; f <= p; f++)
    {
        int page_faults = Simulate(page_trace, f);
        std::cout << std::setw(6) << f << " | " << std::setw(11) << page_faults << std::endl;

        // Save to CSV file for plotting
        results_file << f << "," << page_faults << std::endl;
    }

    results_file.close();
    std::cout << "\nResults saved to 'page_fault_results.csv'" << std::endl;
    std::cout << "Please use Excel or another graphing tool to plot the results." << std::endl;

    return 0;
}

/**
 * PageTraceGenerator subroutine - Generates a random page trace of length n,
 * where the page numbers have values ranging from 0 to p-1.
 */
std::vector<int> PageTraceGenerator(int n, int p)
{
    std::vector<int> trace;
    trace.reserve(n);

    // Set up random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, p - 1);

    // Generate random page numbers
    for (int i = 0; i < n; i++)
    {
        trace.push_back(distrib(gen));
    }

    return trace;
}

/**
 * Simulate subroutine - Accepts a page trace and a parameter f for the number
 * of memory frames available and returns the number of page faults.
 */
int Simulate(const std::vector<int> &page_trace, int f)
{
    // Create page table with entries for each possible page (0 to p-1)
    // Each entry is an unsigned long with:
    // - Bits 0-29: frame_number (0 means not in memory)
    // - Bit 30: present bit (1 if in memory, 0 if not)
    // - Bit 31: reference bit (1 if referenced, 0 if not)
    int p = 0;
    for (const auto &page : page_trace)
    {
        p = std::max(p, page + 1);
    }
    std::vector<unsigned long> page_table(p, 0);

    // Initialize frames_list: array tracking available frames
    std::vector<int> frames_list(f);
    for (int i = 0; i < f; i++)
    {
        frames_list[i] = i + 1; // Store frame numbers 1-based to distinguish from 0 (not in memory)
    }
    int num_frames_avail = f; // Number of available frames initially equals f

    // Track total number of page faults
    int num_page_faults = 0;

    // Process each page in the trace
    for (const auto &current_page : page_trace)
    {
        // Check if page is in memory (present bit set)
        bool is_present = (page_table[current_page] & (1UL << 30)) != 0;

        if (!is_present)
        {
            // Page fault occurred
            num_page_faults++;

            // Need to load page into memory
            if (num_frames_avail > 0)
            {
                // Allocate from available frames
                int frame = frames_list[--num_frames_avail];

                // Update page table: set frame number, present bit, and reference bit
                page_table[current_page] = frame | (1UL << 30) | (1UL << 31);
            }
            else
            {
                // No frames available, need to find a victim to evict
                int victim_page = FindVictim(page_table);

                // Get the frame used by the victim
                int frame = page_table[victim_page] & 0x3FFFFFFF; // Extract frame number (bits 0-29)

                // Update victim page table entry: clear frame number and bits
                page_table[victim_page] = 0;

                // Update current page table entry: set frame number, present bit, and reference bit
                page_table[current_page] = frame | (1UL << 30) | (1UL << 31);
            }
        }
        else
        {
            // Page already in memory, set reference bit
            page_table[current_page] |= (1UL << 31);
        }
    }

    return num_page_faults;
}

/**
 * FindVictim subroutine - This subroutine is given a page table as a parameter,
 * and it chooses and returns a victim page to evict based on the second chance
 * page replacement algorithm.
 */
int FindVictim(std::vector<unsigned long> &page_table)
{
    static int clock_hand = 0;

    while (true)
    {
        // Ensure clock_hand wraps around the page table
        if (clock_hand >= static_cast<int>(page_table.size()))
        {
            clock_hand = 0;
        }

        // Check if current page is in memory (present bit set)
        bool is_present = (page_table[clock_hand] & (1UL << 30)) != 0;

        if (is_present)
        {
            // Check reference bit
            bool is_referenced = (page_table[clock_hand] & (1UL << 31)) != 0;

            if (is_referenced)
            {
                // Give a second chance by clearing reference bit
                page_table[clock_hand] &= ~(1UL << 31); // Clear reference bit
                clock_hand++;                           // Move to next page
            }
            else
            {
                // Found victim - page in memory with reference bit clear
                int victim = clock_hand;
                clock_hand++; // Advance clock hand for next invocation
                return victim;
            }
        }
        else
        {
            // Page not in memory, move to next
            clock_hand++;
        }
    }
}