/**
 *  A.V.Ustinov <austinprog@yandex.ru>
 * The utility downloads two branches from PACKAGE_URL, compare them,
 * and output comparison statistic in JSON format that includes 3 arrays:
 * 1. Which packages is absent in first branch
 * 2. Which packages is absent in second branch
 * 3. All packages in first branch with newer version then in second one
 */
#include <stdio.h>
#include <errno.h>
#include "pcompare.h"

/**
 * @brief main  the main function of the utility
 * @param argc  number of atguments
 * @param argv  pointer to an array of srting arguments
 * @return      SUCCESS code on success, ERROR code otherwise
 */
int main(int argc, char *argv[])
{
    const size_t n_branches_to_compare = N_BRANCHES_TO_COMPARE_SUPPORTED;
    f_param_t fparam[n_branches_to_compare];

    if (argc != (N_BRANCHES_TO_COMPARE_SUPPORTED + 1))
    {
        printf("Please, enter %d names of branches\n", N_BRANCHES_TO_COMPARE_SUPPORTED);
        return ERROR;
    }

    fparam[0].pack_name = argv[1];
    fparam[1].pack_name = argv[2];

    printf("We'll compare package \"%s\" with \"%s\" one\n", fparam[0].pack_name, fparam[1].pack_name);

    /* Load psckages */
    ///int resl = pcompare_load_files(fparam, n_branches_to_compare);
    ///if (resl != SUCCESS)
    ///{
    ///    printf("Load error!\n");
    ///    return resl;
    ///}

    /* Open loading files */
    int res = pcompare_open_downloaded_files(fparam, n_branches_to_compare);
    if (res != SUCCESS)
    {
        printf("Open files error!\n");
        return res;
    }

    /*Compares branches an out result JSON */
    res = pcompare_process_branches(fparam, n_branches_to_compare);

    pcompare_close_files(fparam, n_branches_to_compare);

    return res;
}
