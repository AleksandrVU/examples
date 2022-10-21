#ifndef __PCOMPARE_H_
#define __PCOMPARE_H_
/**
 *  A.V.Ustinov <austinprog@yandex.ru>
 * The header file of the libpcompare library that allows to download two packages' branches ,
 * compare them, and output comparison statistic in JSON format that includes 3 arrays:
 * 1. Which packages is absent in first branch
 * 2. Which packages is absent in second branch
 * 3. All packages in first branch with newer version then in second one
 */

#include <stddef.h>

/**
 * Number of comparing branches supported.
 * At the moment is supported only 2 branches
 */
#define N_BRANCHES_TO_COMPARE_SUPPORTED 2

#define ERROR     -1
#define SUCCESS   0

// structure to store JSON file parameters
typedef struct f_param
{
    const char  *pack_name; //package branch name
    int         fd;         //opened file descripror
    size_t      size;       //file size
    void        *fptr;      //mapped file data pointer
}f_param_t;

/**
 * @brief pcompare_load_files   loads packages information to appropriated file
 * @param fparam                - pointer to a f_param_t structure
 * @param n_branches            - number of branches
 * @return                      SUCCESS on success, ERROR otherwise
 */
int pcompare_load_files(f_param_t *fparam, const size_t n_branches);


/**
 * @brief pcompare_open_downloaded_files    opens and prepares for parsing downloaded fines
 * @param fparam                            pointer to an array of f_param_t structures
 * @param n_files                           number of downloaded files
 * @return                                  SUCCESS on success, ERROR otherwise
 */
int pcompare_open_downloaded_files(f_param_t *fparam, const size_t n_branches);

/**
 * @brief pcompare_close_files  closes files with JSON packages info
 * @param fparam                pointer to an array of f_param_t strictures
 * @param count                 number of opened files
 * @return                      SUCCESS on success, ERROR on invalid input parameter
 */
int pcompare_close_files(f_param_t *fparam, const int count);

/**
 * @brief pcompsre_process_branches     comparing packages' branches and output the result
 * @param fparam                        pointer to an array of f_param_t structures
 * @param n_branches                    number of branches to process
 * @return                              SUCCESS code on success, ERROR code otherwise
 */
int pcompare_process_branches(const f_param_t *fparam, const size_t n_branches);

#endif //__PCOMPARE_H_

