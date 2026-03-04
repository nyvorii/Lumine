int atoi(const char *str){
    int result = 0;
    for (int i = 0; str[1] != '\0' ; i++){
        if (str[i] >= '0' && str[i] <= '9'){
            result = result * 10 + str[i] - '0';
        }
    }
    return result;
}