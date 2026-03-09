#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 职工类型枚举
enum EmployeeType {
    TEACHER = 1,           // 教师
    LAB_STAFF = 2,         // 实验员
    ADMIN = 3,             // 行政人员
    TEACHER_LAB = 4,       // 教师兼职实验员
    ADMIN_TEACHER = 5      // 行政人员兼职教师
};

// 职工信息结构体
typedef struct Employee {
    char id[20];           // 工号
    char name[50];         // 姓名
    int type;              // 职工类型
    int workload;          // 上学期工作量
    double salary;         // 总工资
    struct Employee* next; // 指向下一个职工的指针
} Employee;

// 全局链表头指针
Employee* head = NULL;

// 函数声明
void showMenu();
void addEmployee();
void deleteEmployee();
void searchEmployee();
void displayAll();
void saveToFile();
void loadFromFile();
double calculateSalary(int type, int workload);
int getWorkloadLimit(int type);
Employee* findEmployee(const char* id);
void freeList();

// 主函数
int main() {
    int choice;

    // 程序启动时加载文件数据
    loadFromFile();

    printf("========================================\n");
    printf("     欢迎使用高校工资管理系统\n");
    printf("========================================\n\n");

    while (1) {
        showMenu();
        printf("请输入选项: ");
        scanf("%d", &choice);
        getchar(); // 清除缓冲区的换行符

        switch (choice) {
            case 1:
                addEmployee();
                break;
            case 2:
                deleteEmployee();
                break;
            case 3:
                searchEmployee();
                break;
            case 4:
                displayAll();
                break;
            case 5:
                saveToFile();
                break;
            case 0:
                saveToFile(); // 退出前自动保存
                freeList();   // 释放内存
                printf("\n感谢使用，再见！\n");
                return 0;
            default:
                printf("\n[错误] 无效的选项，请重新输入！\n\n");
        }
    }

    return 0;
}

// 显示菜单
void showMenu() {
    printf("========================================\n");
    printf("            系统主菜单\n");
    printf("========================================\n");
    printf("  1. 录入职工信息\n");
    printf("  2. 删除职工信息\n");
    printf("  3. 查询职工信息\n");
    printf("  4. 显示所有职工\n");
    printf("  5. 保存到文件\n");
    printf("  0. 退出系统\n");
    printf("========================================\n");
}

// 根据职工类型获取基本工作量
int getWorkloadLimit(int type) {
    switch (type) {
        case TEACHER:
        case TEACHER_LAB:
            return 120; // 教师基本工作量120
        case LAB_STAFF:
            return 70;  // 实验员基本工作量70
        case ADMIN:
        case ADMIN_TEACHER:
            return 0;   // 行政人员无基本工作量
        default:
            return 0;
    }
}

// 计算工资
double calculateSalary(int type, int workload) {
    double baseSalary = 0;      // 基本工资
    double allowance = 0;       // 补助/补贴
    double classHourFee = 0;    // 课时费
    int baseWorkload = 0;       // 基本工作量

    switch (type) {
        case TEACHER: // 教师：基本工资 + 课时费
            baseSalary = 800;
            baseWorkload = 120;
            classHourFee = (workload - baseWorkload) * 20;
            return baseSalary + classHourFee;

        case LAB_STAFF: // 实验员：基本工资 + 实验室补助
            baseSalary = 650;
            allowance = 150; // 实验室补助
            return baseSalary + allowance;

        case ADMIN: // 行政人员：基本工资 + 行政补贴
            baseSalary = 750;
            allowance = 250; // 行政补贴
            return baseSalary + allowance;

        case TEACHER_LAB: // 教师兼职实验员：基本工资 + 课时费 + 实验室补助
            baseSalary = 800;
            baseWorkload = 120;
            classHourFee = (workload - baseWorkload) * 20;
            allowance = 150; // 实验室补助
            return baseSalary + classHourFee + allowance;

        case ADMIN_TEACHER: // 行政人员兼职教师：基本工资 + 行政补贴 + 课时费
            baseSalary = 750;
            baseWorkload = 120; // 兼职教师按教师标准
            classHourFee = (workload - baseWorkload) * 20;
            allowance = 250; // 行政补贴
            return baseSalary + allowance + classHourFee;

        default:
            return 0;
    }
}

// 添加职工信息
void addEmployee() {
    Employee* newEmployee = (Employee*)malloc(sizeof(Employee));
    if (newEmployee == NULL) {
        printf("\n[错误] 内存分配失败！\n\n");
        return;
    }

    printf("\n========== 录入职工信息 ==========\n");

    // 输入工号
    printf("请输入工号: ");
    scanf("%s", newEmployee->id);
    getchar();

    // 检查工号是否已存在
    if (findEmployee(newEmployee->id) != NULL) {
        printf("\n[错误] 该工号已存在，请使用其他工号！\n\n");
        free(newEmployee);
        return;
    }

    // 输入姓名
    printf("请输入姓名: ");
    scanf("%s", newEmployee->name);
    getchar();

    // 选择职工类型
    printf("\n职工类型:\n");
    printf("  1. 教师\n");
    printf("  2. 实验员\n");
    printf("  3. 行政人员\n");
    printf("  4. 教师兼职实验员\n");
    printf("  5. 行政人员兼职教师\n");
    printf("请选择类型 (1-5): ");
    scanf("%d", &newEmployee->type);
    getchar();

    // 类型验证
    if (newEmployee->type < 1 || newEmployee->type > 5) {
        printf("\n[错误] 无效的职工类型！\n\n");
        free(newEmployee);
        return;
    }

    // 输入工作量（如果需要）
    if (newEmployee->type == TEACHER ||
        newEmployee->type == TEACHER_LAB ||
        newEmployee->type == ADMIN_TEACHER) {

        while (1) {
            printf("请输入上学期工作量: ");
            scanf("%d", &newEmployee->workload);
            getchar();

            // 工作量验证
            if (newEmployee->workload < 0 || newEmployee->workload > 1000) {
                printf("[错误] 工作量不合法（应在0-1000之间），请重新输入！\n");
            } else {
                break;
            }
        }
    } else if (newEmployee->type == LAB_STAFF) {
        // 实验员不需要录入工作量，工资固定
        newEmployee->workload = 0;
    } else {
        // 行政人员不需要工作量
        newEmployee->workload = 0;
    }

    // 计算工资
    newEmployee->salary = calculateSalary(newEmployee->type, newEmployee->workload);

    // 插入链表头部
    newEmployee->next = head;
    head = newEmployee;

    printf("\n[成功] 职工信息录入成功！\n");
    printf("工号: %s\n", newEmployee->id);
    printf("姓名: %s\n", newEmployee->name);
    printf("工资: %.2f 元\n\n", newEmployee->salary);
}

// 查找职工
Employee* findEmployee(const char* id) {
    Employee* current = head;
    while (current != NULL) {
        if (strcmp(current->id, id) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// 删除职工信息
void deleteEmployee() {
    char id[20];
    printf("\n========== 删除职工信息 ==========\n");
    printf("请输入要删除的职工工号: ");
    scanf("%s", id);
    getchar();

    if (head == NULL) {
        printf("\n[提示] 当前没有任何职工信息！\n\n");
        return;
    }

    // 如果要删除的是头节点
    if (strcmp(head->id, id) == 0) {
        Employee* temp = head;
        head = head->next;
        printf("\n[成功] 已删除职工: %s (%s)\n\n", temp->name, temp->id);
        free(temp);
        return;
    }

    // 查找要删除的节点
    Employee* current = head;
    Employee* prev = NULL;

    while (current != NULL && strcmp(current->id, id) != 0) {
        prev = current;
        current = current->next;
    }

    if (current == NULL) {
        printf("\n[错误] 未找到工号为 %s 的职工！\n\n", id);
        return;
    }

    // 删除节点
    prev->next = current->next;
    printf("\n[成功] 已删除职工: %s (%s)\n\n", current->name, current->id);
    free(current);
}

// 查询职工信息
void searchEmployee() {
    char id[20];
    printf("\n========== 查询职工信息 ==========\n");
    printf("请输入要查询的职工工号: ");
    scanf("%s", id);
    getchar();

    Employee* emp = findEmployee(id);

    if (emp == NULL) {
        printf("\n[错误] 未找到工号为 %s 的职工！\n\n", id);
        return;
    }

    // 显示职工详细信息
    printf("\n-------- 职工详细信息 --------\n");
    printf("工号: %s\n", emp->id);
    printf("姓名: %s\n", emp->name);

    printf("类型: ");
    switch (emp->type) {
        case TEACHER:
            printf("教师\n");
            break;
        case LAB_STAFF:
            printf("实验员\n");
            break;
        case ADMIN:
            printf("行政人员\n");
            break;
        case TEACHER_LAB:
            printf("教师兼职实验员\n");
            break;
        case ADMIN_TEACHER:
            printf("行政人员兼职教师\n");
            break;
    }

    if (emp->type == TEACHER || emp->type == TEACHER_LAB || emp->type == ADMIN_TEACHER) {
        printf("上学期工作量: %d\n", emp->workload);
    }

    printf("总工资: %.2f 元\n", emp->salary);
    printf("-----------------------------\n\n");
}

// 显示所有职工信息
void displayAll() {
    printf("\n========== 所有职工信息 ==========\n");

    if (head == NULL) {
        printf("[提示] 当前没有任何职工信息！\n\n");
        return;
    }

    printf("%-10s %-15s %-20s %-10s %-10s\n",
           "工号", "姓名", "类型", "工作量", "工资(元)");
    printf("------------------------------------------------------------------------\n");

    Employee* current = head;
    while (current != NULL) {
        // 类型名称
        char typeName[25];
        switch (current->type) {
            case TEACHER:
                strcpy(typeName, "教师");
                break;
            case LAB_STAFF:
                strcpy(typeName, "实验员");
                break;
            case ADMIN:
                strcpy(typeName, "行政人员");
                break;
            case TEACHER_LAB:
                strcpy(typeName, "教师兼职实验员");
                break;
            case ADMIN_TEACHER:
                strcpy(typeName, "行政人员兼职教师");
                break;
            default:
                strcpy(typeName, "未知");
        }

        printf("%-10s %-15s %-20s %-10d %.2f\n",
               current->id, current->name, typeName,
               current->workload, current->salary);

        current = current->next;
    }
    printf("\n");
}

// 保存到文件
void saveToFile() {
    FILE* file = fopen("D:\\claude_project\\高校工资管理\\employees.txt", "w");
    if (file == NULL) {
        printf("\n[错误] 无法创建文件！\n\n");
        return;
    }

    Employee* current = head;
    int count = 0;

    while (current != NULL) {
        fprintf(file, "%s %s %d %d %.2f\n",
                current->id, current->name, current->type,
                current->workload, current->salary);
        current = current->next;
        count++;
    }

    fclose(file);
    printf("\n[成功] 已保存 %d 条职工信息到文件！\n\n", count);
}

// 从文件加载
void loadFromFile() {
    FILE* file = fopen("D:\\claude_project\\高校工资管理\\employees.txt", "r");
    if (file == NULL) {
        // 文件不存在是正常情况，首次运行时
        return;
    }

    // 先清空现有链表
    freeList();

    Employee temp;
    int count = 0;

    while (fscanf(file, "%s %s %d %d %lf",
                  temp.id, temp.name, &temp.type,
                  &temp.workload, &temp.salary) == 5) {

        Employee* newEmployee = (Employee*)malloc(sizeof(Employee));
        if (newEmployee == NULL) {
            printf("[错误] 内存分配失败！\n");
            break;
        }

        strcpy(newEmployee->id, temp.id);
        strcpy(newEmployee->name, temp.name);
        newEmployee->type = temp.type;
        newEmployee->workload = temp.workload;
        newEmployee->salary = temp.salary;

        // 插入链表
        newEmployee->next = head;
        head = newEmployee;
        count++;
    }

    fclose(file);

    if (count > 0) {
        printf("[提示] 已从文件加载 %d 条职工信息\n\n", count);
    }
}

// 释放链表内存
void freeList() {
    Employee* current = head;
    while (current != NULL) {
        Employee* temp = current;
        current = current->next;
        free(temp);
    }
    head = NULL;
}
