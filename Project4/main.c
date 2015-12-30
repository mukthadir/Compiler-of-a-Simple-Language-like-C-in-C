//
//  main.c
//  Project4
//
//  Created by Hasiba Choudhury on 4/24/15.
//  Copyright (c) 2015 Mukthadir. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "compiler.h"

#define ALLOC(t) (t*) calloc(1, sizeof(t))


int i = 0;

struct IDfind {
    char idList[1000][200];
    int  idVal[1000];
    int check[1000];
};
struct IDfind find;

int findID(char* );
int findIDforPrint(char* );
int checkLHS(char* );
void initialiseCheck();
int hel();
void checkDup(int , char* );

struct StatementNode* body();
struct StatementNode* parse_stmt_list();
struct AssignmentStatement* asignment_stmt();
struct ValueNode* value_node();
struct StatementNode* parse_stmt();
struct PrintStatement* print_stmt();
struct IfStatement* if_stmt();

void syntax_error(const char* msg)
{
    printf("Syntax error while parsing %s\n", msg);
    exit(1);
}

struct StatementNode* parse_generate_intermediate_representation()
{
        struct StatementNode* statement;
        initialiseCheck();
    
        statement = ALLOC(struct StatementNode);
    
    ttype = getToken();
    while (ttype != LBRACE) {
        ttype = getToken();
    }
    
    ungetToken();
        statement = body();
    return statement;
}

struct StatementNode* body(){
    struct StatementNode* bod;

    bod = ALLOC(struct StatementNode);

    
    ttype = getToken();
    if (ttype == LBRACE)
    {
        bod = parse_stmt_list();
        ttype = getToken();
        if (ttype == RBRACE) {
            //                hel();
            return bod;
        }else
            syntax_error("body. RBRACE expected");
    }else
        syntax_error("body. LBRACE expected");
    return NULL;
}




struct StatementNode* parse_stmt_list(){
    struct StatementNode* st;
    struct StatementNode* stl;
    struct StatementNode* no_Op;
    
    st = ALLOC(struct StatementNode);
    stl = ALLOC(struct StatementNode);
    no_Op = ALLOC(struct StatementNode);
    no_Op->type = NOOP_STMT;
    
    st = parse_stmt();
    ttype = getToken();
    
    if (ttype != RBRACE) {
        ungetToken();
        stl = parse_stmt_list();
        
        if (st->type == IF_STMT) {
            st->next->next = stl;
        } else {
            st->next = stl;
        }
        return st;
    }else{
        ungetToken();
        return st;
    }
}

struct StatementNode* parse_stmt(){
    struct StatementNode* stmt;
    struct StatementNode* stmt1;
    struct StatementNode* no_Op;
    
    stmt = (struct StatementNode*)malloc(sizeof(struct StatementNode)*2);
    stmt1 = (struct StatementNode*)malloc(sizeof(struct StatementNode)*2);
    no_Op = ALLOC(struct StatementNode);
    no_Op->type = NOOP_STMT;

    stmt->if_stmt = ALLOC(struct IfStatement);
    stmt->if_stmt->false_branch = ALLOC(struct StatementNode);
    stmt->if_stmt->true_branch = ALLOC(struct StatementNode);
    stmt->if_stmt->condition_operand1 = ALLOC(struct ValueNode);
    stmt->if_stmt->condition_operand1->name = ALLOC(char);
    stmt->if_stmt->condition_operand2 = ALLOC(struct ValueNode);
    stmt->if_stmt->condition_operand2->name = ALLOC(char);
    
    ttype = getToken();
    if (ttype == ID) {
        ungetToken();
        stmt->type = ASSIGN_STMT;
        stmt->assign_stmt = asignment_stmt();
        ttype = getToken();
        if (ttype == SEMICOLON) {
            return stmt;
        }
    }else if (ttype == PRINT){
        stmt->type = PRINT_STMT;
        stmt->print_stmt = print_stmt();
        ttype = getToken();
        if (ttype == SEMICOLON) {
            return stmt;
        }
    }else if (ttype == IF){
        stmt->type = IF_STMT;
        stmt->if_stmt = if_stmt();
        
        stmt->if_stmt->true_branch = body();
        stmt1 = stmt->if_stmt->true_branch;
        stmt->if_stmt->false_branch = no_Op;
        
        while (stmt1->next != NULL) {
            stmt1 = stmt1->next;
        }
        
        stmt1->next = no_Op;
        
        stmt->next = no_Op;
        
        return stmt;
    }
  else
        syntax_error("parse stmt. SEMICOLON expected.");
    return NULL;
}

struct AssignmentStatement* asignment_stmt(){
    struct AssignmentStatement* asign;
    int flag = -1;
    
    asign = ALLOC(struct AssignmentStatement);
    asign->left_hand_side = ALLOC(struct ValueNode);
    asign->left_hand_side->name = ALLOC(char);
    asign->operand1 = ALLOC(struct ValueNode);
    asign->operand1->name = ALLOC(char);
    asign->operand2 = ALLOC(struct ValueNode);
    asign->operand2->name = ALLOC(char);
    
    ttype = getToken();
    if (ttype == ID) {
        flag = findID(token);
        if (flag != -1) {
            asign->left_hand_side->value = flag;
            find.idVal[i] = flag;
            flag = -1;
        }
        strcpy(find.idList[i++],token);
        strcpy(asign->left_hand_side->name, token);
        find.check[i-1] = 3;
        
        ttype = getToken();
        if (ttype == EQUAL) {
            ttype = getToken();
            if (ttype == NUM) {
                asign->operand1->value = atoi(token);
                
                ttype = getToken();
                if (ttype == SEMICOLON) {
                    asign->left_hand_side->value = asign->operand1->value;
                    asign->op = 0;
                    find.idVal[i-1] = asign->operand1->value;
                    find.check[i-1] = 3;
                    checkDup(asign->left_hand_side->value, asign->left_hand_side->name);
                    ungetToken();
                    return asign;
                }else if (ttype == PLUS){
                    ttype = getToken();
                    if (ttype == NUM) {
                        asign->operand2->value = atoi(token);
                        asign->left_hand_side->value = asign->operand1->value + asign->operand2->value;
                        find.idVal[i-1] = asign->left_hand_side->value;
                        find.check[i-1] = 3;
                        checkDup(asign->left_hand_side->value, asign->left_hand_side->name);
                    }else if (ttype == ID){
                        strcpy(asign->operand2->name,token);
                        if (!strcmp(asign->left_hand_side->name, asign->operand2->name)) {
                            asign->operand2->value = asign->left_hand_side->value;
                        }else{
                            flag = findID(token);
                            if (flag != -1) {
                                asign->operand2->value = flag;
                                flag = -1;
                            }
                        }
                        asign->left_hand_side->value = asign->operand1->value + asign->operand2->value;
                        find.idVal[i-1] = asign->left_hand_side->value;
                        find.check[i-1] = 3;
                        strcpy(find.idList[i++], asign->operand2->name);
                        checkDup(asign->left_hand_side->value, asign->left_hand_side->name);
                    }
                    ttype = getToken();
                    if (ttype == SEMICOLON) {
                        ungetToken();
                        return asign;
                    }
                }else if (ttype == MINUS){
                    ttype = getToken();
                    if (ttype == NUM) {
                        asign->operand2->value = atoi(token);
                        asign->left_hand_side->value = asign->operand1->value - asign->operand2->value;
                        find.idVal[i-1] = asign->left_hand_side->value;
                        find.check[i-1] = 3;
                        checkDup(asign->left_hand_side->value, asign->left_hand_side->name);
                    }else if (ttype == ID){
                        strcpy(asign->operand2->name,token);
                        if (!strcmp(asign->left_hand_side->name, asign->operand2->name)) {
                            asign->operand2->value = asign->left_hand_side->value;
                        }else{
                            flag = findID(token);
                            if (flag != -1) {
                                asign->operand2->value = flag;
                                flag = -1;
                            }
                        }
                        asign->left_hand_side->value = asign->operand1->value - asign->operand2->value;
                        find.idVal[i-1] = asign->left_hand_side->value;
                        find.check[i-1] = 3;
                        strcpy(find.idList[i++], asign->operand2->name);
                        checkDup(asign->left_hand_side->value, asign->left_hand_side->name);
                    }
                    ttype = getToken();
                    if (ttype == SEMICOLON) {
                        ungetToken();
                        return asign;
                    }
                }else if (ttype == MULT){
                    ttype = getToken();
                    if (ttype == NUM) {
                        asign->operand2->value = atoi(token);
                        asign->left_hand_side->value = asign->operand1->value * asign->operand2->value;
                        find.idVal[i-1] = asign->left_hand_side->value;
                        find.check[i-1] = 3;
                        checkDup(asign->left_hand_side->value, asign->left_hand_side->name);
                    }else if (ttype == ID){
                        strcpy(asign->operand2->name,token);
                        if (!strcmp(asign->left_hand_side->name, asign->operand2->name)) {
                            asign->operand2->value = asign->left_hand_side->value;
                        }else{
                            flag = findID(token);
                            if (flag != -1) {
                                asign->operand2->value = flag;
                                flag = -1;
                            }
                        }
                        asign->left_hand_side->value = asign->operand1->value * asign->operand2->value;
                        find.idVal[i-1] = asign->left_hand_side->value;
                        find.check[i-1] = 3;
                        strcpy(find.idList[i++], asign->operand2->name);
                        checkDup(asign->left_hand_side->value, asign->left_hand_side->name);
                    }
                    ttype = getToken();
                    if (ttype == SEMICOLON) {
                        ungetToken();
                        return asign;
                    }
                }else if (ttype == DIV){
                    ttype = getToken();
                    if (ttype == NUM) {
                        asign->operand2->value = atoi(token);
                        asign->left_hand_side->value = asign->operand1->value / asign->operand2->value;
                        find.idVal[i-1] = asign->left_hand_side->value;
                        find.check[i-1] = 3;
                        checkDup(asign->left_hand_side->value, asign->left_hand_side->name);
                    }else if (ttype == ID){
                        strcpy(asign->operand2->name,token);
                        if (!strcmp(asign->left_hand_side->name, asign->operand2->name)) {
                            asign->operand2->value = asign->left_hand_side->value;
                        }else{
                            flag = findID(token);
                            if (flag != -1) {
                                asign->operand2->value = flag;
                                flag = -1;
                            }
                        }
                        asign->left_hand_side->value = asign->operand1->value / asign->operand2->value;
                        find.idVal[i-1] = asign->left_hand_side->value;
                        find.check[i-1] = 3;
                        strcpy(find.idList[i++], asign->operand2->name);
                        checkDup(asign->left_hand_side->value, asign->left_hand_side->name);
                    }
                    ttype = getToken();
                    if (ttype == SEMICOLON) {
                        ungetToken();
                        return asign;
                    }
                }
                
            }else if (ttype == ID){
                strcpy(asign->operand1->name,token);
                if (!strcmp(asign->left_hand_side->name, asign->operand1->name)) {
                    asign->operand1->value = asign->left_hand_side->value;
                }else{
                    flag = findID(token);
                    if (flag != -1) {
                        asign->operand1->value = flag;
                        flag = -1;
                    }
                }
                strcpy(find.idList[i++], asign->operand1->name);
                find.idVal[i-1] = asign->operand1->value;
                checkDup(asign->left_hand_side->value, asign->left_hand_side->name);
                
                
                ttype = getToken();
                if (ttype == SEMICOLON) {
                    asign->left_hand_side->value = asign->operand1->value;
                    asign->op = 0;
                    find.idVal[i-2] = asign->operand1->value;
                    find.check[i-2] = 3;
                    checkDup(asign->left_hand_side->value, asign->left_hand_side->name);
                    ungetToken();
                    return asign;
                }else if (ttype == PLUS){
                    ttype = getToken();
                    if (ttype == NUM) {
                        asign->operand2->value = atoi(token);
                        asign->left_hand_side->value = asign->operand1->value + asign->operand2->value;
                        find.idVal[i-2] = asign->left_hand_side->value;
                        find.check[i-2] = 3;
                        checkDup(asign->left_hand_side->value, asign->left_hand_side->name);
                    }else if (ttype == ID){
                        strcpy(asign->operand2->name,token);
                        if (!strcmp(asign->left_hand_side->name, asign->operand2->name)) {
                            asign->operand2->value = asign->left_hand_side->value;
                        }else{
                            flag = findID(token);
                            if (flag != -1) {
                                asign->operand2->value = flag;
                                flag = -1;
                            }
                        }
                        strcpy(find.idList[i++], asign->operand2->name);
                        asign->left_hand_side->value = asign->operand1->value + asign->operand2->value;
                        find.idVal[i-3] = asign->left_hand_side->value;
                        find.check[i-3] = 3;
                        checkDup(asign->left_hand_side->value, asign->left_hand_side->name);
                    }
                    ttype = getToken();
                    if (ttype == SEMICOLON) {
                        ungetToken();
                        return asign;
                    }
                }else if (ttype == MINUS){
                    ttype = getToken();
                    if (ttype == NUM) {
                        asign->operand2->value = atoi(token);
                        asign->left_hand_side->value = asign->operand1->value - asign->operand2->value;
                        find.idVal[i-2] = asign->left_hand_side->value;
                        find.check[i-2] = 3;
                        checkDup(asign->left_hand_side->value, asign->left_hand_side->name);
                    }else if (ttype == ID){
                        strcpy(asign->operand2->name,token);
                        if (!strcmp(asign->left_hand_side->name, asign->operand2->name)) {
                            asign->operand2->value = asign->left_hand_side->value;
                        }else{
                            flag = findID(token);
                            if (flag != -1) {
                                asign->operand2->value = flag;
                                flag = -1;
                            }
                        }
                        strcpy(find.idList[i++], asign->operand2->name);
                        asign->left_hand_side->value = asign->operand1->value - asign->operand2->value;
                        find.idVal[i-3] = asign->left_hand_side->value;
                        find.check[i-3] = 3;
                        checkDup(asign->left_hand_side->value, asign->left_hand_side->name);
                    }
                    ttype = getToken();
                    if (ttype == SEMICOLON) {
                        ungetToken();
                        return asign;
                    }
                }else if (ttype == MULT){
                    ttype = getToken();
                    if (ttype == NUM) {
                        asign->operand2->value = atoi(token);
                        asign->left_hand_side->value = asign->operand1->value * asign->operand2->value;
                        find.idVal[i-2] = asign->left_hand_side->value;
                        find.check[i-2] = 3;
                        checkDup(asign->left_hand_side->value, asign->left_hand_side->name);
                    }else if (ttype == ID){
                        strcpy(asign->operand2->name,token);
                        if (!strcmp(asign->left_hand_side->name, asign->operand2->name)) {
                            asign->operand2->value = asign->left_hand_side->value;
                        }else{
                            flag = findID(token);
                            if (flag != -1) {
                                asign->operand2->value = flag;
                                flag = -1;
                            }
                        }
                        strcpy(find.idList[i++], asign->operand2->name);
                        asign->left_hand_side->value = asign->operand1->value * asign->operand2->value;
                        find.idVal[i-3] = asign->left_hand_side->value;
                        find.check[i-3] = 3;
                        checkDup(asign->left_hand_side->value, asign->left_hand_side->name);
                    }
                    ttype = getToken();
                    if (ttype == SEMICOLON) {
                        ungetToken();
                        return asign;
                    }
                }else if (ttype == DIV){
                    ttype = getToken();
                    if (ttype == NUM) {
                        asign->operand2->value = atoi(token);
                        asign->left_hand_side->value = asign->operand1->value / asign->operand2->value;
                        find.idVal[i-2] = asign->left_hand_side->value;
                        find.check[i-2] = 3;
                        checkDup(asign->left_hand_side->value, asign->left_hand_side->name);
                    }else if (ttype == ID){
                        strcpy(asign->operand2->name,token);
                        if (!strcmp(asign->left_hand_side->name, asign->operand2->name)) {
                            asign->operand2->value = asign->left_hand_side->value;
                        }else{
                            flag = findID(token);
                            if (flag != -1) {
                                asign->operand2->value = flag;
                                flag = -1;
                            }
                        }
                        strcpy(find.idList[i++], asign->operand2->name);
                        asign->left_hand_side->value = asign->operand1->value / asign->operand2->value;
                        find.idVal[i-3] = asign->left_hand_side->value;
                        find.check[i-3] = 3;
                        
                        checkDup(asign->left_hand_side->value, asign->left_hand_side->name);
                    }
                    ttype = getToken();
                    if (ttype == SEMICOLON) {
                        ungetToken();
                        return asign;
                    }
                }
            }
        }else if (ttype == SEMICOLON){
            ungetToken();
            return asign;
        }
    }else
        ungetToken();
    return asign;
}

struct ValueNode* value_node(){
    struct ValueNode* val;
    int flag;
    
    val = ALLOC(struct ValueNode);
    val->name = ALLOC(char);
    
    ttype = getToken();
    if (ttype == ID) {
        strcpy(find.idList[i++],token);
        strcpy(val->name, token);
        flag = findID(token);
        if (flag) {
            val->value = flag;
        }
    }else if (ttype == NUM){
        
        
        val->value = atoi(token);
    }
    
    return  val;
}


struct PrintStatement* print_stmt(){
    struct PrintStatement* prt;
    
    prt = ALLOC(struct PrintStatement);
    prt->id = ALLOC(struct ValueNode);
    ttype = getToken();
    if (ttype == ID) {
        prt->id->value = findIDforPrint(token);
    }

    return prt;
}

struct IfStatement* if_stmt(){
    struct IfStatement* ifstmt;
    int flag = -1;
    
    ifstmt = ALLOC(struct IfStatement);
    ifstmt->condition_operand1 = ALLOC(struct ValueNode);
    ifstmt->condition_operand1->name = ALLOC(char);
    ifstmt->condition_operand2 = ALLOC(struct ValueNode);
    ifstmt->condition_operand2->name = ALLOC(char);
    
    ttype = getToken();
    
    if (ttype == NUM) {
        ifstmt->condition_operand1->value = atoi(token);
    }else if (ttype == ID){
        strcpy(ifstmt->condition_operand1->name, token);
        
        flag = findID(token);
        if (flag != -1) {
            ifstmt->condition_operand1->value = flag;
            flag = -1;
        }
        strcpy(find.idList[i++], ifstmt->condition_operand1->name);
        checkDup(ifstmt->condition_operand1->value, ifstmt->condition_operand1->name);
    }
    
    ttype = getToken();
    if (ttype == GREATER || ttype == LESS || ttype == NOTEQUAL) {
        ifstmt->condition_op = ttype;
    }
    
    ttype = getToken();
    if (ttype == NUM) {
        ifstmt->condition_operand2->value = atoi(token);
    }else if (ttype == ID){
        strcpy(ifstmt->condition_operand2->name, token);
        
        flag = findID(token);
        if (flag != -1) {
            ifstmt->condition_operand2->value = flag;
            flag = -1;
        }
        strcpy(find.idList[i++], ifstmt->condition_operand2->name);
        checkDup(ifstmt->condition_operand2->value, ifstmt->condition_operand2->name);
    }
    
    return ifstmt;
}

int findID(char* token){
    int x = 0;

    for (x = 0; x<i; x++) {
        if (!strcmp(token, find.idList[x])) {
            return find.idVal[x];
        }
    }
    return -1;
}

int findIDforPrint(char* token){
    int x = 0;
    for (x = 0; x<i; x++) {
        if (!strcmp(token, find.idList[x])) {
            find.check[x] = 1;
            
            return find.idVal[x];
        }
    }
    return 0;
}

void initialiseCheck(){
    int z = 0;
    for(z = 0;z<1000;z++){
        find.check[z] = 0;
    }
}

int checkLHS(char* token){
    int x = 0;
    for (x=0; x<i; x++) {
        if (!strcmp(token, find.idList[x])) {
            return x;
        }
    }
    return -1;
}

int hel(){
    int x = 0;
    for (x = 0; x<i; x++) {
        printf("values are %d %s\n",find.idVal[x],find.idList[x]);
    }
    return 0;
}

void checkDup(int val, char* name){
    int x = 0;
    for (x = 0; x<i; x++) {
        if (!strcmp(name, find.idList[x])) {
            find.idVal[x] = val;
        }
    }
}


