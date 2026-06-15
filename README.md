# Mini Student Shell

C 언어로 구현한 학생 데이터 관리 셸 프로그램. 연결 리스트로 학생 정보를 관리하고 CSV 파일에 저장합니다.

## 빌드

```bash
make admin    # admin_shell 빌드 (-DADMIN_MODE)
make client   # client_shell 빌드 (-DCLIENT_MODE)
make all      # 두 프로그램 모두 빌드
make clean    # 빌드 파일 삭제
```

## 실행

```bash
./admin_shell students.csv
./client_shell students.csv

# 명령어 파일 사용 (-f 옵션)
./admin_shell students.csv -f commands.txt
./client_shell students.csv -f commands.txt
```

- `-f` 명령어 파일 처리 후 `exit` 없이 끝나면 프로그램이 종료됩니다.
- `-f` 옵션으로 지정한 명령어 파일을 열 수 없는 경우 오류 메시지를 출력하고 종료합니다.

## 명령어

| 명령어 | Admin | Client | 설명 |
|--------|-------|--------|------|
| `list` | O | O | 전체 학생 목록 출력 |
| `find <id>` | O | O | ID로 학생 검색 |
| `sort <name\|score>` | O | X | 이름 또는 점수로 정렬 |
| `help` | O | O | 명령어 도움말 |
| `exit` | O | O | 프로그램 종료 |
| `reload` | O | O | CSV에서 다시 불러오기 |
| `save` | O | X | CSV에 저장 |
| `add <id> <name> <score>` | O | X | 학생 추가 |
| `delete <id>` | O | X | 학생 삭제 |
| `update <id> <score>` | O | X | 점수 수정 |

## CSV 형식

```
id,name,score
1,Alice,90
2,Bob,85
3,Charlie,95
```

- `id`: 양의 정수, 고유값
- `name`: 공백·쉼표 없음, 최대 31자
- `score`: 0~100 정수

## 명령어 파일 형식

- 한 줄에 하나의 명령어
- `#`으로 시작하는 줄은 주석으로 무시
- 명령어 실행 시 `[command file:N]` 형태로 줄 번호 출력
- 오류 발생 시 해당 줄 건너뛰고 다음 명령어 계속 실행

```
# 예시 commands.txt
list
add 4 David 88
update 99 70
find 4
save
exit
```

## 파일 구조

```
project/
├── main.c       - 프로그램 시작, 인자 처리, 셸 루프
├── student.h    - Student 구조체와 linked list 함수 선언
├── student.c    - linked list 구현
├── file_io.h    - CSV load/save 함수 선언
├── file_io.c    - CSV 파일 입출력 구현
├── command.h    - command 타입, command table, handler 선언
├── command.c    - 명령어 파싱과 command handler 구현
├── Makefile     - 빌드 자동화
├── README.md    - 프로젝트 설명과 사용법
├── students.csv - 샘플 CSV 파일
├── commands.txt - 샘플 명령어 파일
├── report.md    - 프로젝트 보고서 (마크다운 형식)
└── report.pdf   - 프로젝트 보고서 (pdf 형식)

```

## 제출 전 기능 확인

```bash
python3 grader.py ./admin_shell ./client_shell students.csv
```
