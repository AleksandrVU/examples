/**
 *  A.V.Ustinov <austinprog@yandex.ru>
 * The utility downloads two branches from PACKAGE_URL, compare them,
 * and oitput comparison statistic in JSON format that includes 3 arrays:
 * 1. Which packages is absent in first branch
 * 2. Which packages is absent in second branch
 * 3. All packages in first branch with newer version then in second one
 */
#include <stdio.h>
#include <json-c/json.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>
#include <curl/curl.h>
#include <errno.h>

#define PACKAGE_URL "https://rdb.altlinux.org/api/export/branch_binary_packages/"
#define PACKAGE1                        "p9"
#define PACKAGE2                        "p10"
#define EQUAL                           0
#define N_BRANCHES_TO_COMPARE_SUPPORTED 2       //number of comparing branches supported
#define ERROR                           -1
#define SUCCESS                         0
#define MAX_FILE_NAME_LEN               128
#define MAX_COMMAND_LEN                 256
#define HEADER_STR_LEN                  64
#define N_BRANCHES_TO_CHECK_VERSION     1       // number of branches to check
#define BRANCH_TO_CHECK_VERSION         0       // branch number to check newer wersion

const char *ARCH_TAG     = "arch";
const char *NAME_TAG     = "name";
const char *VERSION_TAG  = "version";
const char *PACKAGES_TAG = "packages";

// structure to store JSON file parameters
typedef struct f_param
{
    const char  *pack_name; //package branch name
    int         fd;         //opened file descripror
    size_t      size;       //file size
    void        *fptr;      //mapped file data pointer
    json_object *wrapper;   //main JSON object pointer
 }f_param_t;

//structure to store branches comparison statistic
typedef struct
{
    size_t *absent_packages_indexes[N_BRANCHES_TO_COMPARE_SUPPORTED];   //stores package indexes of absent packages in current branch
    size_t *version_indexes[N_BRANCHES_TO_CHECK_VERSION];               //stores package indexes with newer version
    size_t index_couters[N_BRANCHES_TO_COMPARE_SUPPORTED];              //counters of absent packages
    size_t version_counter;                                             //counter of first packages with newer version value
    size_t n_branches;                                                  //number of branches to compare (for the release it 2)
}branches_statistic_t;

// structure to store JSON packets array information
typedef struct
{
    json_object *packages_array;        //pointer to an array of packets info
    size_t      packages_array_length;  //the array length
}json_packages_t;

//Codes inicates of pacjages array scanning
typedef enum
{
    GO_AHEAD = 0,
    FIRST_FINISHED,
    SECOND_FINISHED,
    BOTH_FINISHED
}continue_result_t;

/**
 * @brief close_files   unmap and close files with JSON packages info
 * @param fparam    pointer to an array of f_param_t strictures
 * @param count     number of opened files
 */
static void close_files(f_param_t *fparam, const int count)
{
    for ( int i =0; i < count; ++i)
    {
        if (fparam[i].fptr)
        {
            munmap(fparam[i].fptr, fparam[i].size);
            fparam[i].fptr = NULL;
        }
        if (fparam[i].fd>=0)
        {
            close(fparam[i].fd);
            fparam[i].fd = -1;
        }
    }

}

/**
 * @brief map_file  maps loaded files
 * @param fparam    pointer to a f_param_t structure
 * @return          SUCCESS on success, ERROR otherwise
 */
static int map_file(f_param_t * fparam)
{
    int fd;
    char fname[MAX_FILE_NAME_LEN]="";
    sprintf(fname,"%s.json",fparam->pack_name);

    fd = open(fname, O_RDONLY);
    if (fd < 0)
    {
        printf("File \"%s\" open error. Reason: %s\n", fname, strerror(errno));
        return fd;
    }
    struct stat f_stat;
    int res = fstat(fd, &f_stat);
    if (res < 0)
    {
        printf("File \"%s\" get statistic error\n", fname);
        close(fd);
        return  res;
    }

    void *fptr = mmap(NULL, f_stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (!fptr)
    {
        printf("File \"%s\" mapping error\n", fname);
        close(fd);
        return  ERROR;
    }
    fparam->fd = fd;
    fparam->fptr = fptr;
    fparam->size = f_stat.st_size;

    return SUCCESS;
}

/**
 * @brief json_file_parse   JSON file parsing
 * @param param             pointer to a f_param_t structure
 * @return                  SUCCESS on success, ERROR otherwise
 */
void * json_file_parse(void * param)
{
    f_param_t *fparam = (f_param_t*)param;

    printf("Parsing \"%s\" file...\n", fparam->pack_name);

    fparam->wrapper = json_tokener_parse(fparam->fptr);
    printf("\"%s\" file parsing finished.\n", fparam->pack_name);

    return NULL;
}

/**
 * @brief json_load loads branch information to a file
 * @param fparam    pointer to a f_param_t structure
 * @return          SUCCESS on success, ERROR otherwise
 */
int json_load(const f_param_t *fparam)
{
    char url[MAX_COMMAND_LEN];
    char fname[MAX_COMMAND_LEN];

    sprintf(url,"%s/%s", PACKAGE_URL, fparam->pack_name);
    sprintf(fname,"%s.json",fparam->pack_name);

    FILE *f = fopen(fname,"wb");
    if (!f)
    {
        printf("Could not open \"%s\"file to write\n", fname);
        return ERROR;
    }

    char curlErrorBuffer[CURL_ERROR_SIZE];
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl = curl_easy_init();
    if (!curl)
    {
        printf("Packet %s: CURL init error!\n", fparam->pack_name);
        fclose(f);
        return ERROR;
    }
    /* Error buffer definition */
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, curlErrorBuffer);

    /* URL download definition */
    curl_easy_setopt(curl, CURLOPT_URL, url);

    /* Switch HTML header off */
    curl_easy_setopt(curl, CURLOPT_HEADER, 0L);

    /* Set file descriptor as a buffer to write */
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, f);

    /* Switch show progress statistic on */
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);

    /* Auto redirecting enable */
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    printf("\nLoading packet \"%s\"...\n", fparam->pack_name);
    CURLcode curlResult = curl_easy_perform(curl);

    fclose(f);
    curl_easy_cleanup(curl);

    if (curlResult != CURLE_OK)
    {
        printf("Package \"%s\"Curl perfom error = %d. Error message:\"%s\"\n",fparam->pack_name, curlResult, curlErrorBuffer);
        return ERROR;
    }
    printf("\nPacket \"%s\" loading finished\n", fparam->pack_name);

    return SUCCESS;
}

/**
 * @brief load_packages  loads packages information to appropriated file
 * @param fparam        - pointer to a f_param_t structure
 * @param n_branches    - number of branches
 * @return              SUCCESS on success, ERROR otherwise
 */
int load_packages(f_param_t *fparam, const size_t n_branches)
{
    for (size_t i=0; i < n_branches; ++i)
    {
       int res = json_load(&fparam[i]);
       if (res != SUCCESS)
       {
           close_files(fparam, i);
           return res;
       }
    }
    return SUCCESS;
}

/**
 * @brief mapping_files maps files with loaded JSON structures
 * @param fparam        - pointer to f_param_t structure
 * @param n_branches    - number of branches
 * @return              SUCCESS on success, ERROR otherwise
 */
int mapping_files(f_param_t *fparam, const size_t n_branches)
{
    for (size_t i=0; i < n_branches; ++i)
    {
        int res = map_file(&fparam[i]);
        if (res != SUCCESS)
        {
            printf("Mapping \"%s.json\" file error\n", fparam->pack_name);
            close_files(fparam, i);
            return res;
        }
    }
    return SUCCESS;
}

/**
 * @brief parsing_json_files - parsing JSON files in separate threads
 * @param fparam            - pointer to f_param_t structure
 * @param n_branches        - number of branches
 * @return                  SUCCESS on success, ERROR otherwise
 */
int parsing_json_files(f_param_t *fparam, const size_t n_branches)
{
    pthread_t parse_thread[n_branches];
    size_t i = 0;
    int res = SUCCESS;
    for (; i < n_branches; ++i)
    {
        pthread_create(&parse_thread[i], NULL, json_file_parse, &fparam[i]);
    }
    for (i = 0; i < n_branches; ++i)
    {
        pthread_join(parse_thread[i], NULL);
        if(!fparam[i].wrapper)
            res = ERROR;
    }

    return res;
}

/**
 * @brief compare_tags  - compare 2 json strings values
 * @param iters          - pointer to an array of json objects
 * @param n_branches    - number of branches to compare, suppotred only for 2 branches
 * @param tag           - pointer to a tag string
 * @return              result of strcmp function
 */
int compare_tags(json_object **iters, const size_t n_branches, const char *tag)
{
    json_object * tag_obj[n_branches];
    for( size_t j = 0; j < n_branches; ++j)
            tag_obj[j] = json_object_object_get(iters[j], tag);
    const char * tag_val[n_branches];
    for( size_t j = 0; j < n_branches; ++j)
        tag_val[j] = json_object_get_string(tag_obj[j]);

    int res = strcmp(tag_val[0], tag_val[1]); //released compare for 2 strings only!
    return res;
}

/**
 * @brief next_position_depend_on_compare_result    shifts next position in packages array depending on compared packages' names result
 * @param res                                       comparison result
 * @param counters                                  pointer to an array of packages' arrays current indexes positions
 * @param packages_info                             pointer to an array of json_packages_t structures
 * @return                                          continue_result_t code
 */
continue_result_t next_position_depend_on_compare_result(const int res, size_t *counters, const json_packages_t *packages_info)// limit for 2 branches only!
{
    continue_result_t cont_result = GO_AHEAD;
    if (counters[0] >= (packages_info[0].packages_array_length - 1))
    {
        cont_result = FIRST_FINISHED;
    }
    else if (res <= 0)
    {
        ++counters[0];
    }
    if (counters[1] >= (packages_info[1].packages_array_length - 1))
    {
        cont_result |= SECOND_FINISHED;
    }
    else if (res >= 0)
    {
        ++counters[1];
    }
    return cont_result;
}

#define N_OUT_PARAMS    3
/**
 * @brief compare_out - output comparible branches packages, the function is for debug only
 * @param iter        - pointer to an array of branches packet JSON objects
 * @param n_branches  - number of branches to compare
 */
void compare_out(const json_object **iter, const size_t n_branches)
{
    const char *tags_to_out[N_OUT_PARAMS] = {NAME_TAG, VERSION_TAG, ARCH_TAG};
    printf("\n{\n");
    for (size_t i = 0; i < N_OUT_PARAMS; ++i)
    {
        for (size_t j = 0; j < n_branches; ++j)
        {
            json_object * cobj = json_object_object_get(iter[j], tags_to_out[i]);
            const char *str =  json_object_get_string(cobj);
            printf("%32s", str);
        }
        printf("\n");
    }
    printf("}\n");
}

/**
 * @brief free_arrays_memory    free allocated memory for arrays
 * @param arr                   pointer to an array of allocated arrays
 * @param n_arrays              number of arrays
 */
void free_arrays_memory(size_t **arr, const size_t n_arrays)
{
    for (size_t i = 0; i < n_arrays; ++i)
    {
        free (arr[i]);
        arr[i] = 0;
    }
}

/**
 * @brief init_arrays   - allocates memory for arrays
 * @param arr           - array of pointers to arrays that wil be allocated
 * @param packages_info - array of json_packages_t structures
 * @param n_arrays      - number of array to initiate
 * @return              SUCCESS code on success, ERROR otherwise
 */
int init_arrays(size_t **arr, const json_packages_t *packages_info, const size_t n_arrays)
{
    for (size_t i = 0; i < n_arrays; ++i)
    {
        arr[i] = calloc(packages_info[i].packages_array_length, sizeof (size_t));
        if (!arr[i])
        {
            printf("init_arrays: Memory allocation error for %lu array\n", i);
            free_arrays_memory(arr, i);
            return ERROR;
        }
    }
    return SUCCESS;
}

/**
 * @brief destroy_branch_statistic  calls release memory for allocated arrays
 * @param stat                      pointer to branches_statistic_t structure
 */
void destroy_branch_statistic(branches_statistic_t *stat)
{
    free_arrays_memory(stat->absent_packages_indexes, stat->n_branches);
    free_arrays_memory(stat->version_indexes, N_BRANCHES_TO_CHECK_VERSION);
}

/**
 * @brief init_branch_statistic     initiates btanches statistic structure and allocates memory for its arrays
 * @param stat                      pointer to branches_statistic_t structure
 * @param packages_info             pointer to an array of json_packages_t structures
 * @param n_branches                number of branches to process
 * @return                      SUCCESS code on success, ERROR code otherwise
 */
static int init_branch_statistic(branches_statistic_t *stat, const json_packages_t *packages_info, const size_t n_branches)
{

    int res = init_arrays(stat->absent_packages_indexes, packages_info, n_branches);
    if (res!=SUCCESS) return res;
    for (size_t i = 0; i < N_BRANCHES_TO_COMPARE_SUPPORTED; ++i )
    {
        stat->index_couters[i] = 0;
    }
    res = init_arrays(stat->version_indexes, &packages_info[BRANCH_TO_CHECK_VERSION], N_BRANCHES_TO_CHECK_VERSION);
    if (res!=SUCCESS)
    {
        free_arrays_memory(stat->absent_packages_indexes, n_branches);
        return res;
    }
    stat->version_counter = 0;
    stat->n_branches = n_branches;

    return SUCCESS;
}

/**
 * @brief get_packages_info  gets information about packages arrays and their length
 * @param wrapper           pointer to a main JSON object
 * @param info              pointer to a json_packages_t structure
 * @return                  SUCCESS code on success, ERROR code otherwise
 */
static int get_packages_info(const json_object *wrapper, json_packages_t *info)
{
    info->packages_array = json_object_object_get(wrapper, PACKAGES_TAG);
    if (!info->packages_array)
    {
        printf("Couln't find packages array!");
        return ERROR;
    }

    info->packages_array_length = json_object_array_length(info->packages_array);
    return SUCCESS;
}

/**
 * @brief update_branches_statistic - stores index of absent package depend on compare result
 * @param stat                      pointer to branches_statistic_t structure
 * @param res                       comparison result of packages names
 * @param counters                  package counters array
 */
static void update_branches_statistic(branches_statistic_t *stat, const int res, const size_t *counters)  //NOTE: for 2 branches only
{
    if (res < EQUAL)    //if package in first branch absent in second
    {
        stat->absent_packages_indexes[1][stat->index_couters[1]] = counters[0];
        ++stat->index_couters[1];
        return;
    }
    //if package in second branch absent in first
    stat->absent_packages_indexes[0][stat->index_couters[0]] = counters[1];
    ++stat->index_couters[0];
}

/**
 * @brief update_version_statistic  - stores indexes of packages in branch BRANCH_TO_CHECK_VERSION with newer vestion then other
 * @param stat                  pointer to branches_statistic_t structure
 * @param res                   comparison result of packages names
 * @param counters              package counters array
 */
static void update_version_statistic(branches_statistic_t *stat, const int res, const size_t *counters)//NOTE: for 2 branches only and Version1 > Vesrion2 condition
{
    if (res < EQUAL) return;
    stat->version_indexes[BRANCH_TO_CHECK_VERSION][stat->version_counter] = counters[BRANCH_TO_CHECK_VERSION];
    ++stat->version_counter;
}

/**
 * @brief get_branches_statistic    scans and compares branches' packages and store scanning statistic
 * @param packages_info             pointer to an array of packages_info structure
 * @param branches_statistic        pointer to branches_statistic_t structure
 * @return                          SUCCESS code on success, ERROR code otherwise
 */
static int get_branches_statistic(const json_packages_t *packages_info, branches_statistic_t *branches_statistic)
{
    const size_t n_branches = branches_statistic->n_branches;
    json_object * iters[n_branches];
    size_t counters[n_branches];
    size_t i;
    continue_result_t can_continue = GO_AHEAD;

    for (i = 0; i < n_branches; ++i) counters[i] = 0;
    int res;
    while (1)
    {
        for( size_t j = 0; j < n_branches; ++j)
        {
            iters[j]= json_object_array_get_idx(packages_info[j].packages_array, counters[j]);
            if (!iters[j])
            {
                printf("No iteraror for %lu branch, on %lu step!\n", j, counters[j]);
                return ERROR;
            }
        }
        switch (can_continue)
        {
            case GO_AHEAD:
                res = compare_tags(iters, n_branches, NAME_TAG);
                break;
            case FIRST_FINISHED:
                res = 1;
                break;
            case SECOND_FINISHED:
                res = -1;
                break;
            default:
                printf("Unexpected code = %d\n", can_continue);
                return ERROR;
        }

        if ( res == EQUAL )
        {
            res = compare_tags(iters, n_branches, VERSION_TAG);
            if (res != EQUAL) //if versions is different
            {
                update_version_statistic(branches_statistic, res, counters);
            }
            res = EQUAL;
        }
        else
        {
            update_branches_statistic(branches_statistic, res, counters);
        }
        can_continue = next_position_depend_on_compare_result(res, counters, packages_info);
        if (can_continue == BOTH_FINISHED) break;
    }
    return SUCCESS;
}

/**
 * @brief out_statistic_array   output packages statistic in JSON array
 * @param header                header(name) of JSON array
 * @param index_array           array of indexes to output
 * @param length                length of output array
 * @param packages              pointer to an array of packages to output
 */
static void out_statistic_array(const char *header, const size_t *index_array, const size_t length, const json_object *packages)
{
    const char *tags_to_out[N_OUT_PARAMS] = {NAME_TAG, VERSION_TAG, ARCH_TAG};

    printf("\"length\": %lu,\n", length);
    printf("%s",header);
    for (size_t i = 0; i < length; )
    {
        printf("{\n");
        size_t ind = index_array[i];
        json_object *iter = json_object_array_get_idx(packages, ind);
        for (size_t k = 0; k < N_OUT_PARAMS; )
        {
            json_object * cobj = json_object_object_get(iter, tags_to_out[k]);
            const char *str =  json_object_get_string(cobj);
            printf("    \"%s\":\"%s\"", tags_to_out[k], str);
            ++k;
            if (k < N_OUT_PARAMS) printf(",");
            printf("\n");
        }
        printf("}");
        ++i;
        if (i < length) printf(",");
        printf("\n");
    }

}


/**
 * @brief out_branches_statistic    output branches comparison statistic. (NOTE - released for 2 branches only!)
 * @param fparam                    pointer to an array of f_param_t structures
 * @param packages_info             pointer to an array of json_packages_t structures
 * @param stat                      pointer to a branches_statistic_t structure
 */
static void out_branches_statistic(const f_param_t *fparam, const json_packages_t *packages_info, const branches_statistic_t *stat)
{

    printf("{\n");
    const size_t n_branches = stat->n_branches;
    char header_str[HEADER_STR_LEN];
    int sl;
    for(size_t i = 0; i < n_branches; ++i)
    {
        size_t length = stat->index_couters[i];
        size_t branch_with_absent_pack = i^1; //for two branches 1 and 0 indexes valid
        json_object *packages_array =  packages_info[branch_with_absent_pack].packages_array;
        sl = sprintf(header_str, "\"absent_in_%s_packages\":[\n",fparam[i].pack_name);
        header_str[sl] = 0;

        out_statistic_array(header_str, stat->absent_packages_indexes[i], length, packages_array);
         printf("],\n");
    }
    sl = sprintf(header_str, "\"%s_packages_newer_versions\":[\n",fparam[BRANCH_TO_CHECK_VERSION].pack_name);
    header_str[sl] = 0;
    out_statistic_array(header_str, stat->version_indexes[BRANCH_TO_CHECK_VERSION],
                        stat->version_counter, packages_info[BRANCH_TO_CHECK_VERSION].packages_array);
    printf("]\n");
    printf("}\n");
}

/**
 * @brief process_branches  comparing packages' branches and output the result
 * @param fparam            pointer to an array of f_param_t structures
 * @param n_branches        number of branches to process
 * @return                  SUCCESS code on success, ERROR code otherwise
 */
static int process_branches(const f_param_t *fparam, const size_t n_branches)
{
    json_packages_t packages_info[n_branches];
    size_t i = 0;

    for (; i < n_branches; ++i)
    {
        if (get_packages_info(fparam[i].wrapper, &packages_info[i]) != SUCCESS)
        {
            return ERROR;
        }
    }

    branches_statistic_t branches_statistic;

    if (init_branch_statistic(&branches_statistic, packages_info, n_branches) != SUCCESS)
    {
        printf("Init branches statistic error!\n");
        return ERROR;
    }

    if (get_branches_statistic(packages_info, &branches_statistic) != SUCCESS)
    {
        printf("Get branches statistic error!\n");
        destroy_branch_statistic(&branches_statistic);
        return ERROR;
    }
    out_branches_statistic(fparam, packages_info, &branches_statistic);
    destroy_branch_statistic(&branches_statistic);
    return SUCCESS;
}

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

    if (argc < (N_BRANCHES_TO_COMPARE_SUPPORTED + 1))
        printf("Please, enter %d names of branches\n", N_BRANCHES_TO_COMPARE_SUPPORTED);

    fparam[0].pack_name = argv[1];
    fparam[1].pack_name = argv[2];

    printf("We'll compare package \"%s\" with \"%s\" one\n", fparam[0].pack_name, fparam[1].pack_name);

    /* Packages loading */
    int resl = load_packages(fparam, n_branches_to_compare);
    if (resl != SUCCESS)
    {
        printf("Load error!\n");
        return resl;
    }

    /* Mapping loading files */
    int res = mapping_files(fparam, n_branches_to_compare);
    if (res != SUCCESS)
    {
        printf("Mapping error!\n");
        return res;
    }

    /* Parsing packages files */
    res = parsing_json_files(fparam, n_branches_to_compare);
    if (res != SUCCESS)
    {
        printf("Parsing error!\n");
        close_files(fparam, n_branches_to_compare);
        return res;
    }
    /*Compares branches an out result JSON */
    res = process_branches(fparam, n_branches_to_compare);


    close_files(fparam, n_branches_to_compare);

    return res;
}
