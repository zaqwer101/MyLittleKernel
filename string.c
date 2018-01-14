#define TRUE 1 
#define FALSE 0 

int compare_strings(const char *str1, const char *str2, int len)
{
    for(int i=0; i < len; i++)
    {
        if(str1[i] != str2[i])
            return FALSE;
    }
    //if (str2[f + 1] != "\0")
    //    return FALSE;
    return TRUE;
}
