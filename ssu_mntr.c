#include "ssu_mntr.h"

char pwd[BUFFER_SIZE];
int option_i = false;
int option_r = false;
int option_d = false;
int option_l = false;

void ssu_mntr(void) // 프롬프트 메인 함수
{
	// 프롬프트
	char check_path[BUFFER_SIZE]; // $(PWD)/check 절대경로
	char command_line[MAX_BUFFER_SIZE]; // 입력받은 실행 명령 버퍼

	// 명령어
	commands command;
	int command_type = false; // 실행 명령 타입
	char target_path[BUFFER_SIZE]; // FINE_NAME 경로

	// 공통
	file_node *head;
	int idx;

	// DELETE

	// SIZE
	int number;

	// RECOVER

	// TREE
	int level_check[BUFFER_SIZE];

	getcwd(pwd, BUFFER_SIZE);
	sprintf(check_path, "%s/%s", pwd, CHECK); // 모니터링 디렉토리 경로 추출

	while (command_type != EXIT) {

		fputs(PROMPT, stdout); // 프롬프트 라인 출력, 20162448> 
		fgets(command_line, sizeof(command_line), stdin); // 실행 명령 입력
		strcpy(command_line, ltrim(rtrim(command_line))); // 실행 명령 좌우 공백 지우기
		command = make_command_token(command_line); 
		command_type = get_command_type(command.argv[0]); // 명령 타입 구분

		// COMMANDS
		// DELETE(1)  : DELETE  <FILENAME> <END_TIME> [OPTION]
		// SIZE(2)    : SIZE    <FILENAME> [OPTION]
		// RECOVER(3) : RECOVER <FILENAME> [OPTION]
		// TREE(4)    : TREE
		// EXIT(5)    : EXKT
		// HELP(6)    : HELP

		switch(command_type) {
			case DELETE:

				if(command.argc < 2 || (command.argv[1][0] == '-' && command.argc == 2)) { // 인자 개수가 부족할 경우
					fprintf(stderr, "%s: FILE_NAME doesn't exist", command.argv[0]);
					continue;
				}

				if(access(command.argv[1], F_OK) < 0) { // 파일이 존재하지 않을 경우
					fprintf(stderr, "%s: access error for %s\n", command.argv[0], command.argv[1]);
					continue;
				}

				realpath(command.argv[1], target_path); // FILE_NAME을 절대 경로로 변경

				head = make_list(check_path);
				/*
				   if((head = is_file_exist(head, target_path)) == NULL) { // 해당 파일 탐색, 존재시 해당 노드, 존재하지 않으면 NULL
				   fprintf(stderr, "%s: %s doesn't exist", command.argv[1]);
				   continue;
				   }
				 */



				break;

			case SIZE:

				number = true;

				if(command.argc < 2 || (command.argv[1][0] == '-' && command.argc == 2)) { // FILE_NAME이 주어지지 않은 경우
					fprintf(stderr, "%s: FILE_NAME doesn't exist\n", command.argv[0]);
					continue;
				}

				if(access(command.argv[1], F_OK) < 0) { // 파일이 존재하지 않을 경우
					fprintf(stderr, "%s: access error for %s\n", command.argv[0], command.argv[1]);
					continue;
				}

				if(command.argc > 2) { // 인자가 2개 이상일 경우
					if(!strcmp(command.argv[2], "-d")) { // -d 옵션이 존재하는 경우

						if(command.argc < 4) { // -d 옵션이 존재하고 인자의 개수가 부족한 경우
							fprintf(stderr, "%s: NUMBER doesn't exist\n", command.argv[0]);
							continue;
						} else if((number = atoi(command.argv[3])) == 0) { // -d옵션이 존재하고 NUMBER가 올바르지 않은 경우
							fprintf(stderr, "%s: invalid input NUMBER\n", command.argv[0]);
							continue;
						}

						option_d = true; // -d 옵션 확인

					} else { // 인자가 제대로 주어지지 않은 경우
						fprintf(stderr, "%s: invalid input OPTION\n", command.argv[0]);
						continue;
					}
				}

				realpath(command.argv[1], target_path); // FILE_NAME을 절대 경로로 변환 

				head = make_list(target_path); // 해당 경로의 파일 목록 구조체 생성
				print_list_size(head, target_path, number, true); // 출력
				free_list(head);

				break;

			case RECOVER:
				break;

			case TREE:

				memset(level_check, 0, sizeof(level_check));

				head = make_list(check_path); // 모니터링 디렉토리 파일 목록 구조체 생성
				print_list_tree(head, 0, level_check, true); // 출력 
				//free_list(head); // 메모리 할당 해제
				break;

			case EXIT:
				break;

			case HELP:
			case UNKNOWN:

				print_usage();
				break;

			default:
				continue;
		}

		init_option();
		//memset(target_path, 0, sizeof(char));
		//memset(command_line, 0, sizeof(char));

	}
	fprintf(stdout, "Good bye...\n");
	fflush(stdout); // 표준 출력 스트림을 비움
	return;
}

commands make_command_token(char *command_line) // 명령어 전체 문장 토큰화
{
	commands result;
	char *tmp;
	char *command;

	result.argv = (char **)malloc(sizeof(char*) * BUFFER_SIZE);
	result.argc = 0;

	if((command = strtok(command_line, " ")) == NULL) { // 엔터만 쳤을 경우
		result.argv[result.argc] = (char *)malloc(sizeof(char));
		strcpy(result.argv[result.argc], "");
		return result;
	}

	to_lower_case(command); // 명령어 소문자화
	result.argv[result.argc] = (char *)malloc(sizeof(char) * strlen(command)); // 메모리 공간 할당
	strcpy(result.argv[result.argc++], command); // 토큰 배열에 복사

	while((tmp = strtok(NULL, " ")) != NULL) { // 나머지 인자 복사
		result.argv[result.argc] = (char *)malloc(sizeof(char) * strlen(command)); // 메모리 공간 할당
		strcpy(result.argv[result.argc++], tmp); // 토큰 배열에 복사
	}

	return result;
}

int get_command_type(char *command) // COMMAND 타입 확인 및 반환
{
	// 명령어 타입 확인
	if(!strcmp(command,""))
		return false;
	else if(!strcmp(command, "delete"))
		return DELETE;
	else if(!strcmp(command, "size"))
		return SIZE;
	else if(!strcmp(command, "recover"))
		return RECOVER;
	else if(!strcmp(command, "tree"))
		return TREE;
	else if(!strcmp(command, "exit"))
		return EXIT;
	else if(!strcmp(command, "help"))
		return HELP;
	else
		return UNKNOWN;
}

void remove_directory(char *path) // 디렉토리 삭제
{
	// trash와 info에 저장하는 코드 작성 필요!
	struct dirent *dirp;
	struct stat statbuf;
	DIR *dp;
	char tmp[MAX_BUFFER_SIZE];

	if((dp = opendir(path)) == NULL)
		return;

	while((dirp = readdir(dp)) != NULL) { // path에 존재하는 디렉토리 안에 파일들 전부 삭제
		if(!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, ".."))
			continue;

		sprintf(tmp, "%s/%s", path, dirp->d_name); // tmp = 디렉토리 내부 파일

		if(lstat(tmp, &statbuf) == -1) // 파일 상태 정보 추출
			continue;

		if(S_ISDIR(statbuf.st_mode)) // 디렉토리일 경우 재귀적으로 제거
			remove_directory(tmp);
		else
			unlink(tmp);
	}

	closedir(dp);
	remove_directory(path);
}

void print_list_size(file_node *head, char *path, int number, int op_switch) // 지정 파일 상대 경로 및 크기 출력
{
	char *relative_path;
	file_node *now;

	now = head;

	while(number > 0) {

		relative_path = now->name + strlen(pwd); // 상대 경로 추출
		printf("%-10ld.%-s\n", now->attr.st_size, relative_path); // 출력

		if(op_switch) {
			if(option_d) { // 옵션이 주어졌을 경우
				if(!S_ISDIR(now->attr.st_mode))  // 파일인 경우
					break; // 탐색 종료
			} else break; // 옵션이 주어지지 않은 경우
		}

		op_switch = false;

		if(now->child != NULL) // 하위 디렉토리 파일들이 존재하면 
			print_list_size(now->child, path, number - 1, op_switch); // 하위 디렉토리 파일 출력 

		if(now->next != NULL) // 같은 레벨에 파일들이 더 존재할 경우
			now = now->next;
		else break; // 탐색 종료
	}
}

void print_list_tree(file_node *head, int level, int level_check[], int is_root) // 모니터링 파일 목록 트리 출력
{
	file_node *now;
	char file_name[BUFFER_SIZE];
	char *tmp;

	now = head;

	while(true) {
		strcpy(file_name, strtok(now->name, "/")); // 파일명 추출
		while((tmp = strtok(NULL, "/")) != NULL) {
			strcpy(file_name, tmp);
		}

		if(is_root) { // 루트 디렉토리 노드일 경우 디렉토리 이름만 출력 후 하위 파일 노드로 이동
			printf("%s\n", file_name); // 파일명 출력
			now = now->child; // 하위 파일 노드로 이동
			is_root = false; // 루트 확인 해제
			level_check[level++] = true; // 레벨 증가
			continue;
		}

		print_indent(level, level_check);

		if(now->next != NULL) 
			printf("├──%s\n", file_name);
		else
			printf("└──%s\n", file_name);


		if(S_ISDIR(now->attr.st_mode)) { // 같은 레벨의 파일이 디렉토리일 경우
			if(now->next != NULL) { // 같은 레벨의 파일들이 더 존재할 경우
				if(now->child != NULL) {
					level_check[level] = true;
					print_list_tree(now->child, level + 1, level_check, is_root);
					level_check[level] = false;
				}
			} else  // 같은 레벨의 파일들이 더 존재하지 않을 경우
				if(now->child != NULL) 
					print_list_tree(now->child, level + 1, level_check, is_root);
		}

		if(now->next != NULL)
			now = now->next;

		else break;
	}	
}

void print_indent(int level, int level_check[]) // 트리 출력 보조 함수
{
	int i;

	for(i = 1; i < level; i++) {
		if(level_check[i])
			printf("│  ");
		else
			printf("   ");
	}
}

char *rtrim(char *_str) // 문자열 오른쪽 공백 제거
{
	char tmp[MAX_BUFFER_SIZE];
	char *end;

	strcpy(tmp, _str);
	end = tmp + strlen(tmp) - 1;
	while(end != _str && isspace(*end))
		--end;

	*(end + 1) = '\0';
	_str = tmp;
	return _str;
}

char *ltrim(char *_str) // 문자열 왼쪽 공백 제거
{
	char *start = _str;

	while(*start != '\0' && isspace(*start))
		++start;
	_str = start;
	return _str;
}

void to_lower_case(char *str) // 문자열 소문자 변환
{
	int i = 0;

	while(str[i]) {
		if(str[i] >= 'A' && str[i] <= 'Z'){
			str[i] = str[i]+32;
		}
		i++;
	}
}

void init_option(void) // 옵션 확인 초기화
{
	option_i = false;
	option_r = false;
	option_d = false;
	option_l = false;
}

void print_usage(void) // 사용법 출력
{
	printf("Usage : [COMMAND]\n");
	printf("Command : \n");
	printf(" DELETE  <FILENAME> <END_TIME> [OPTION]     delete the file at the END_TIME\n");
	printf(" Option :  -i	                            delete file without moving 'trash' directory\n");
	printf("           -r                               re-confirm when the specified time, delete or not to delete\n\n");
	printf(" SIZE    <FILENAME> [OPTION]                print relative file path and size\n");
	printf(" Option :  -d  <NUMBER>                     print as much as NUMBER level\n\n");
	printf(" RECOVER <FILENAME> [OPTION]                restore deleted files to their original path\n");
	printf(" Option :  -l                               print file list in 'trash' directory sort by oldest deletion time before command execute\n\n");
	printf(" TREE                                       print a list of monitoring files in a tree format.\n");
	printf(" EXIT                                       program exit\n");
	printf(" HELP                                       print usage\n");
}
