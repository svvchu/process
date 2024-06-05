//2-1까지 진행

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <windows.h>

typedef struct Process {
	int pid;
	bool isForeground;
	struct Process* next;
} Process;

typedef struct StackNode {
	Process* processList;
	struct StackNode* next;
} StackNode;

typedef struct DynamicQueue {
	StackNode* top;
	int threshold;
} DynamicQueue;


void split_n_merge(DynamicQueue* dq);
void promote(DynamicQueue* dq, StackNode* node);


void initQueue(DynamicQueue* dq, int threshold) {
	dq->top = (StackNode*)malloc(sizeof(StackNode));
	dq->top->processList = NULL;
	dq->top->next = NULL;
	dq->threshold = threshold;
}


void enqueue(DynamicQueue* dq, Process* process) {
	if (dq->top == NULL) {
		initQueue(dq, dq->threshold);
	}

	if (process->isForeground) {
		process->next = dq->top->processList;
		dq->top->processList = process;
	}
	else {
		Process* temp = dq->top->processList;
		if (temp == NULL) {
			dq->top->processList = process;
			process->next = NULL;
		}
		else {
			while (temp->next != NULL) {
				temp = temp->next;
			}
			temp->next = process;
			process->next = NULL;
		}
	}

	
	split_n_merge(dq);
}


Process* dequeue(DynamicQueue* dq) {
	if (dq->top == NULL || dq->top->processList == NULL) return NULL;

	Process* process = dq->top->processList;
	dq->top->processList = dq->top->processList->next;

	if (dq->top->processList == NULL) {
		StackNode* temp = dq->top;
		dq->top = dq->top->next;
		free(temp);
	}

	return process;
}


void promote(DynamicQueue* dq, StackNode* node) {
	if (node == NULL || node->processList == NULL) return;

	StackNode* temp = dq->top;
	if (temp == node) return; 

	while (temp->next != NULL && temp->next != node) {
		temp = temp->next;
	}

	if (temp->next == NULL) return; 

	temp->next = node->next;
	node->next = dq->top;
	dq->top = node;
}


void split_n_merge(DynamicQueue* dq) {
	StackNode* node = dq->top;

	while (node != NULL) {
		Process* process = node->processList;
		int count = 0;
		while (process != NULL) {
			count++;
			process = process->next;
		}

		if (count > dq->threshold) {
			Process* second_half = node->processList;
			for (int i = 0; i < count / 2 - 1; i++) {
				second_half = second_half->next;
			}
			Process* temp = second_half->next;
			second_half->next = NULL;

			StackNode* new_node = (StackNode*)malloc(sizeof(StackNode));
			new_node->processList = temp;
			new_node->next = node->next;
			node->next = new_node;

			promote(dq, new_node);
		}

		node = node->next;
	}
}


void printQueue(DynamicQueue* dq) {
	StackNode* current = dq->top;
	while (current != NULL) {
		Process* process = current->processList;
		while (process != NULL) {
			printf("%d%s ", process->pid, process->isForeground ? "F" : "B");
			process = process->next;
		}
		printf("\n");
		current = current->next;
	}
}


int main() {
	DynamicQueue dq;
	initQueue(&dq, 3);

	Process* p1 = (Process*)malloc(sizeof(Process));
	p1->pid = 1;
	p1->isForeground = true;
	enqueue(&dq, p1);

	Process* p2 = (Process*)malloc(sizeof(Process));
	p2->pid = 2;
	p2->isForeground = false;
	enqueue(&dq, p2);

	Process* p3 = (Process*)malloc(sizeof(Process));
	p3->pid = 3;
	p3->isForeground = false;
	enqueue(&dq, p3);

	Process* p4 = (Process*)malloc(sizeof(Process));
	p4->pid = 4;
	p4->isForeground = true;
	enqueue(&dq, p4);

	printQueue(&dq);

	Process* p = dequeue(&dq);
	printf("Dequeued: %d%s\n", p->pid, p->isForeground ? "F" : "B");
	free(p);

	printQueue(&dq);

	return 0;
}
