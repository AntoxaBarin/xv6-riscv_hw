#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

#define BUFSIZE 128
#define DEPTH_LIMIT_EXIT_CODE -10
#define NOT_EXIST_FILE_CODE -20

// Just calls sys_symlink
int create_link(const char *filename, const char *target, const char *test_num) {
    if (symlink(filename, target) < 0) {
        fprintf(2, "Test %s. Failed to create symlink: %s -> %s.\n", test_num, target, filename);
        return -1;
    }
    return 0;
}

// Creates file and write into it
int create_file(const char *filename, const char *data, const char *test_num) {
    int fd = open(filename, O_CREATE | O_WRONLY);
	if (fd < 0) {
		fprintf(2, "Test %s. Failed to create file %s.\n", test_num, filename);
		return -1;
	}
	if (write(fd, data, strlen(data)) < 0) {
		fprintf(2, "Test %s. Failed to write data to file %s.\n", test_num, filename);
		unlink(filename);
		return -2;
	}
	close(fd);
	return 0;
}

// Compare file's data with arg data
int check_link_content(const char* path, const char* data, const char* test_num) {
	char buf[BUFSIZE];
    int read_res;

    int fd = open(path, 0);
	if (fd < 0) {
		fprintf(2, "Test %s. Failed to open symlink %s.\n", test_num, path);
		return -4;
	}
	if ((read_res = read(fd, buf, BUFSIZE - 1)) < 0) {
		fprintf(2, "Test %s. Failed to read data from link's target.\n", test_num);
		close(fd);
		return -5;
	}
	close(fd);
	buf[read_res] = 0;
	if (strcmp(buf, data)) {
		fprintf(2, "Test %s. Incorrect data.\n", test_num);
		return -6;
	}
	return 0;
}

// Validate symlink
int test_link(const char *filename, const char *target, const char *data, const char *test_num) {
    if (create_file(filename, data, test_num) < 0) {
		return -1;
	}
	if (create_link(filename, target, test_num) < 0) {
		unlink(filename);
		return -2;
	}
	if (check_link_content(target, data, test_num) < 0) {
		unlink(filename);
		unlink(target);
		return -3;
	}
	unlink(filename);
	unlink(target);
	return 0;
}

/* 1. Корректная абсолютная ссылка на файл. */
int test_1() {
	if (test_link("/dir/f1", "/dir/sl1", "FileData1", "1") < 0) {
        fprintf(2, "Test 1. Fail.\n");
        return -1;    
    }
    fprintf(2, "Test 1. Success.\n");
    return 0;
}

/* 2. Корректная относительная ссылка на файл того же каталога. */
int test_2() {
	if (test_link("./f2", "sl2", "FileData2", "2") < 0) {
        fprintf(2, "Test 2. Fail.\n");
        return -1;
    }
    fprintf(2, "Test 2. Success.\n");
    return 0;
}

/* Корректная относительная ссылка на файл каталога на 2-3 уровня выше. */
int test_3() {
    int prep = mkdir("dir/dir2") | mkdir("dir/dir2/dir3");
    if (prep != 0) {
        fprintf(2, "Test 3. Failed to create directories.\n");
        return -1;
    } 
    int test_res = test_link("./dir/dir2/dir3/f3", "sl3", "FileData3", "3");
	unlink("dir/dir2/dir3");
	unlink("dir/dir2");

    if (test_res == 0) {
        fprintf(2, "Test 3. Success.\n");
        return 0;
    }
    fprintf(2, "Test 3. Fail.\n");
	return test_res;
}

/* Корректная относительная ссылка на файл каталога на 2-3 уровня ниже. */
int test_4() {
    int prep = mkdir("dir/dir2") | mkdir("dir/dir2/dir3");
    if (prep != 0) {
        fprintf(2, "Test 4. Failed to create directories.\n");
        return -1;
    } 

    int test_res = test_link("../../../f4", "./dir/dir2/dir3/sl4", "FileData4", "4");
	unlink("dir/dir1/dir2");
	unlink("dir/dir2");
    
    if (test_res == 0) {
        fprintf(2, "Test 4. Success.\n");
        return 0;
    }
    fprintf(2, "Test 4. Fail.\n");
	return test_res;
}

/* Корректная абсолютная ссылка на абсолютную символическую
	ссылку (глубина рекурсии 2-3, конечная цель - существующий файл). */
int test_5() {
	const char* file = "/dir/dir5_2/dir5_3/f5";
	const char* sl1 = "/dir/dir5_2/sl5_1" ;
	const char* sl2 = "/dir/dir5_2/dir5_3/sl5_2" ;
	const char* sl3 = "/dir/sl5_3";
	const char* data = "FileData5";
    const char *test_num = "5";
    #define unlink_all unlink(file); unlink(sl1); unlink(sl2); unlink(sl3);
    #define unlink_paths unlink("dir/dir5_2/dir5_3"); unlink("dir/dir5_2");

    int prep = mkdir("dir/dir5_2") | mkdir("dir/dir5_2/dir5_3");
    if (prep != 0) {
        fprintf(2, "Test 5. Failed to create directories.\n");
		unlink_paths
        return -1;
    } 
	if (create_file(file, data, test_num) != 0) {
		unlink_paths
		return -2;
	}
	if (create_link(file, sl1, test_num) != 0) {
		unlink(file);
		unlink_paths
		return -3;
	}
	if (create_link(sl1, sl2, test_num) != 0) {
		unlink(file);
		unlink(sl1);
		unlink_paths
		return -3;
	}
	if (create_link(sl2, sl3, test_num) != 0) {
		unlink(file);
		unlink(sl1);
		unlink(sl2);
		unlink_paths
		return -3;
	}
	if (check_link_content(sl3, data, test_num) != 0) {
        unlink_all
		unlink_paths
		return -4;
	}
	if (check_link_content(sl2, data, test_num) != 0) {
		unlink_all
		unlink_paths
		return -4;
	}
	if (check_link_content(sl1, data, test_num) != 0) {
		unlink_all
		unlink_paths
		return -4;
	}

	unlink_all
	unlink_paths
	fprintf(2, "Test 5. Success.\n");
	return 0;

    #undef unlink_all
    #undef unlink_paths
}

/* Корректная абсолютная ссылка на относительную символическую ссылку 
   (глубина рекурсии 2-3, конечная цель - существующий файл). */
int test_6() {
	const char* file = "dir6_3/f6";
	const char* abs_file = "/dir/dir6_2/dir6_3/f6";
	const char* sl1 = "/dir/dir6_2/sl6_1";
	const char* sl2 = "dir6_2/dir6_3/sl6_2";
	const char* abs_s2 = "/dir/dir6_2/dir6_3/sl6_2";
	const char* sl3 = "/dir/sl6_3";
	const char* data = "FileData6";
	const char* test_num = "6";
    #define unlink_all unlink(abs_file); unlink(sl1); unlink(abs_s2); unlink(sl3);
    #define unlink_paths unlink("dir/dir6_2/dir6_3"); unlink("dir/dir6_2");

    int prep = mkdir("dir/dir6_2") | mkdir("dir/dir6_2/dir6_3");
    if (prep != 0) {
        fprintf(2, "Test 6. Failed to create directories.\n");
		unlink_paths
        return -1;
    } 
	if (create_file(abs_file, data, test_num) != 0) {
		unlink_paths
		return -2;
	}
	if (create_link(file, sl1, test_num) != 0) {
		unlink(abs_file);
		unlink_paths
		return -3;
	}
	if (create_link(sl1, abs_s2, test_num) != 0) {
		unlink(abs_file);
		unlink(sl1);
		unlink_paths
		return -3;
	}
	if (create_link(sl2, sl3, test_num) != 0) {
		unlink(abs_file);
		unlink(sl1);
		unlink(abs_s2);
		unlink_paths
		return -3;
	}
	if (check_link_content(sl3, data, test_num) != 0) {
		unlink_all
		unlink_paths
		return -4;
	}
	if (check_link_content(abs_s2, data, test_num) != 0) {
		unlink_all
		unlink_paths
		return -4;
	}
	if (check_link_content(sl1, data, test_num) != 0) {
		unlink_all
		unlink_paths
		return -4;
	}

	unlink_all
	unlink_paths
	fprintf(2, "Test 6. Success.\n");
	return 0;

    #undef unlink_all
    #undef unlink_paths
}

/* Корректная относительная ссылка на относительную символическую ссылку 
   (глубина рекурсии 2-3, конечная цель - существующий файл). */
int test_7() {
	const char* file = "f7";
	const char* abs_file = "/dir/dir7_2/f7";
	const char* abs_s1 = "/dir/dir7_2/sl7_1";
	const char* s1 = "dir7_2/sl7_1";
	const char* abs_s2 = "/dir/sl7_2";
	const char* s2 = "../../sl7_2";
	const char* abs_s3 = "/dir/dir7_2/dir7_3/sl7_3";
	const char* data = "FileData7";
	const char* test_num = "7";
    #define unlink_paths unlink("dir/dir7_2/dir7_3"); unlink("dir/dir7_2");
    #define unlink_all unlink(abs_file); unlink(abs_s1); unlink(abs_s2); unlink(abs_s3);

    int prep = mkdir("dir/dir7_2") | mkdir("dir/dir7_2/dir7_3");
    if (prep != 0) {
        fprintf(2, "Test 7. Failed to create directories.\n");
		unlink_paths
        return -1;
    } 
	if (create_file(abs_file, data, test_num) != 0) {
		unlink_paths
		return -2;
	}
	if (create_link(file, abs_s1, test_num) != 0) {
		unlink(abs_file);
		unlink_paths
		return -3;
	}
	if (create_link(s1, abs_s2, test_num) != 0) {
		unlink(abs_file);
		unlink(abs_s1);
		unlink_paths
		return -3;
	}
	if (create_link(s2, abs_s3, test_num) != 0) {
		unlink(abs_file);
		unlink(abs_s1);
		unlink(abs_s2);
		unlink_paths
		return -3;
	}
	if (check_link_content(abs_s3, data, test_num) != 0) {
		unlink_all
		unlink_paths
		return -4;
	}
	if (check_link_content(abs_s2, data, test_num) != 0) {
		unlink_all
		unlink_paths
		return -4;
	}
	if (check_link_content(abs_s1, data, test_num) != 0) {
		unlink_all
		unlink_paths
		return -4;
	}

	unlink_all
	unlink_paths
	fprintf(2, "Test 7. Success.\n");
	return 0;

    #undef unlink_paths
    #undef unlink_all

}

/* Ссылка на себя (бесконечная рекурсия) */
int test_8() {
    const char* abs_s1 = "/dir/sl8";
    const char *test_num = "8";
    
    if (create_link(abs_s1, abs_s1, test_num) != 0) {
        fprintf(2, "Test 8-2. Failed to create directories.\n");
		return -3;
	}
    if (open(abs_s1, O_RDONLY) != DEPTH_LIMIT_EXIT_CODE) {
        fprintf(2, "Test 8-3. Failed to create directories.\n");
		unlink(abs_s1);
		return -4;
	}

    unlink(abs_s1);
	fprintf(2, "Test 8. Success.\n");
    return 0;

}

/* Косвенная ссылка на себя (бесконечная рекурсия через 2-3 перехода) */
int test_9() {
	const char* abs_s1 = "/dir/dir9_2/sl9_1";
	const char* abs_s2 = "/dir/sl9_2";
	const char* abs_s3 = "/dir/dir9_2/dir9_3/sl9_3";
	const char* data = "FileData9";
	const char* test_num = "9";
    #define unlink_paths unlink("dir/dir9_2/dir9_3"); unlink("dir/dir9_2");
    #define unlink_all unlink(abs_s1); unlink(abs_s2); unlink(abs_s3);


    int prep = mkdir("dir/dir9_2") | mkdir("dir/dir9_2/dir9_3");
    if (prep != 0) {
        fprintf(2, "Test 9. Failed to create directories.\n");
		unlink_paths
        return -1;
    } 
	if (create_file(abs_s1, data, test_num) != 0) {
		unlink_paths
		return -2;
	}
	if (create_link(abs_s1, abs_s2, test_num) != 0) {
		unlink(abs_s1);
		unlink_paths
		return -3;
	}
	if (create_link(abs_s2, abs_s3, test_num) != 0) {
		unlink(abs_s1);
		unlink(abs_s2);
		unlink_paths
		return -3;
	}
	unlink(abs_s1);
	if (create_link(abs_s3, abs_s1, test_num) != 0) {
		unlink(abs_s2);
		unlink(abs_s3);
		unlink_paths
		return -3;
	}
	if (open(abs_s1, O_RDONLY) != DEPTH_LIMIT_EXIT_CODE) {
		unlink_all
		unlink_paths
		return -5;
	}
	if (open(abs_s2, O_RDONLY) != DEPTH_LIMIT_EXIT_CODE) {
		unlink_all
		unlink_paths
		return -5;
	}
	if (open(abs_s3, O_RDONLY) != DEPTH_LIMIT_EXIT_CODE) {
		unlink_all
		unlink_paths
		return -5;
	}

	unlink_all
	unlink_paths
	fprintf(2, "Test 9. Success.\n");
	return 0;

    #undef unlink_paths
    #undef unlink_all
}

/* Абсолютная ссылка на несуществующий файл */
int test_10() {	
	const char* abs_file = "/dir/dir10_2/dir10_3/f9";
	const char* abs_s1 = "/dir/dir10_2/sl9";
	const char* data = "FileData10";
	const char* test_num = "10";
    #define unlink_paths unlink("dir/dir10_2/dir10_3"); unlink("dir/dir10_2");

    int prep = mkdir("dir/dir10_2") | mkdir("dir/dir10_2/dir10_3");
    if (prep != 0) {
        fprintf(2, "Test 10. Failed to create directories.\n");
        unlink_paths
        return -1;
    } 
	if (create_file(abs_file, data, test_num) != 0) {
		unlink_paths
		return -2;
	}
	if (create_link(abs_file, abs_s1, test_num) != 0) {
		unlink(abs_s1);
		unlink_paths
		return -3;
	}
	unlink(abs_file);
	if (open(abs_s1, O_RDONLY) != NOT_EXIST_FILE_CODE) {
		fprintf(2, "Test %s. Failed.\n", test_num);
		unlink(abs_s1);
		unlink_paths
		return -5;
	}
	
	unlink(abs_s1);
	unlink_paths
	fprintf(2, "Test %s. Success.\n", test_num);
	return 0;

    #undef unlink_paths
}

/* Относительная ссылка на несуществующий файл того же каталога 
   (при этом файл существует в каталоге на 2-3 уровня выше или ниже) */
int test_11() {
	const char* abs_file = "/dir/t112/t113/f10";
	const char* file = "t112/t113/f10";
	const char* abs_file2 = "/dir/f10";
	const char* as1 = "/dir/l101";
	const char* data = "FileData11";
	const char* testname = "11";

    #define unlink_paths unlink("dir/t112/t112"); unlink("dir/t112");
    #define unlink_all unlink(abs_file); unlink(abs_file2);

    int prep = mkdir("dir/t112") | mkdir("dir/t112/t113");
    if (prep != 0) {
        fprintf(2, "Test %s. Failed to create directories.\n", testname);
		unlink_paths
        return -1;
    } 
	if (create_file(abs_file, data, testname) != 0) {
		unlink_paths
		return -2;
	}
	if (create_file(abs_file2, data, testname) != 0) {
		unlink_paths
		return -2;
	}
	if (create_link(file, as1, testname) != 0) {
		unlink_all
		unlink_paths
		return -3;
	}
	unlink(abs_file);
	if (open(as1, O_RDONLY) != NOT_EXIST_FILE_CODE) {
		fprintf(2, "Test %s. Failed.\n", testname);
		unlink(as1);
		unlink_paths
		return -5;
	}
	
	unlink_all
	unlink_paths
	fprintf(2, "Test 11. Success.\n");
	return 0;

    #undef unlink_paths
    #undef unlink_all
}

/* Относительная ссылка на несуществующий файл каталога
   на 2-3 уровня выше (при этом файл в том же каталоге существует) */
int test_12() {
	const char* afp1 = "/dir/t122/t123/f121";
	const char* afp2 = "/dir/f121";
	const char* fp2 = "f121";
	const char* as1 = "/dir/l121";
	const char* data = "FileData12";
	const char* test_num = "12";
    #define unlink_paths unlink("dir/t122/t123"); unlink("dir/t122");
    #define unlink_all unlink(afp1); unlink(afp2);

    int prep = mkdir("dir/t122") | mkdir("dir/t122/t123");
    if (prep != 0) {
        fprintf(2, "Test\t%s. Failed to create directories.\n", test_num);
		unlink_paths
        return -1;
    } 
	if (create_file(afp1, data, test_num) != 0) {
		unlink_paths
		return -2;
	}
	if (create_file(afp2, data, test_num) != 0) {
		unlink_paths
		return -2;
	}
	if (create_link(fp2, as1, test_num) != 0) {
		unlink_all
		unlink_paths
		return -3;
	}
	unlink(afp2);
	if (open(as1, O_RDONLY) != NOT_EXIST_FILE_CODE) {
		fprintf(2, "Test %s. Failed.\n", test_num);
		unlink(as1);
		unlink_paths
		return -5;
	}
	
	unlink_all
	unlink_paths
	fprintf(2, "Test %s. Success.\n", test_num);
	return 0;

    #undef unlink_paths
    #undef unlink_all
}

/* Относительная ссылка на несуществующий файл каталога
	на 2-3 уровня ниже (при этом файл в том же каталоге существует) */
int test_13() {
	const char* afp1 = "/dir/t132/t133/f121";
	const char* afp2 = "/dir/f121";
	const char* fp2 = "../../f121";
	const char* as1 = "/dir/t132/t133/l121";
	const char* data = "FuleData13";
	const char* test_num = "12";
    #define unlink_paths unlink("dir/t132/t133"); unlink("dir/t132");
    #define unlink_all unlink(afp1); unlink(afp2);

    int prep = mkdir("dir/t132") | mkdir("dir/t132/t133");
    if (prep != 0) {
        fprintf(2, "Test %s. Failed to create directories.\n", test_num);
		unlink_paths
        return -1;
    } 
	if (create_file(afp1, data, test_num) != 0) {
		unlink_paths
		return -2;
	}
	if (create_file(afp2, data, test_num) != 0) {
		unlink_paths
		return -2;
	}
	if (create_link(fp2, as1, test_num) != 0) {
		unlink_all
		unlink_paths
		return -3;
	}
	unlink(afp2);
	if (open(as1, O_RDONLY) != NOT_EXIST_FILE_CODE) {
		fprintf(2, "Test %s. Failed.\n", test_num);
		unlink(as1);
		unlink_paths
		return -5;
	}
	
    unlink_all
	unlink_paths
	fprintf(2, "Test 13. Success.\n");
	return 0;

    #undef unlink_paths
    #undef unlink_all

}

int main() {
    if (mkdir("dir")) {
        fprintf(2, "Failed to create directory dir for testing.\n");
        exit(1);
    }
    
	int test_res_sum = 0;
	test_res_sum += test_1();
	test_res_sum += test_2();
	test_res_sum += test_3();
	test_res_sum += test_4();
    test_res_sum += test_5();
    test_res_sum += test_6();
    test_res_sum += test_7();
    test_res_sum += test_8();
    test_res_sum += test_9();
    test_res_sum += test_10();
    test_res_sum += test_11();
    test_res_sum += test_12();
    test_res_sum += test_13();
	
    unlink("dir");

	if (!test_res_sum) {
		printf("All tests successed.\n");
	}
	exit(0);
}