#define _CRT_SECURE_NO_WARNINGS 1
#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include<string.h>

typedef struct info
{
	char character;
	int weight;
}info;
typedef struct tree_node
{
	info val;
	struct tree_node* left;
	struct tree_node* right;
}tree_node;
tree_node* tree_node_init(info val,tree_node* left,tree_node* right)
{
	tree_node* ret = (tree_node*)malloc(sizeof(tree_node));
	if (ret != NULL)
	{
		ret->val = val;
		ret->left = left;
		ret->right = right;
	}
	return ret;
}
typedef struct chain_queue_node
{
	tree_node* val;
	struct chain_queue_node* next;
}chain_queue_node;

typedef struct chain_queue//无头结点，单向循环链表
{
	chain_queue_node* rear;
	size_t size;
}chain_queue;

chain_queue* chain_queue_init()
{
	chain_queue* ret = (chain_queue*)malloc(sizeof(chain_queue));
	if (ret != NULL)
	{
		ret->rear = NULL;
		ret->size = 0;
	}
	return ret;
}
int chain_queue_push(chain_queue* q, tree_node* val)
{
	if (q == NULL)
	{
		return -1;
	}
	chain_queue_node* new_node = (chain_queue_node*)malloc(sizeof(chain_queue_node));
	if (new_node == NULL)
	{
		return -1;
	}
	new_node->val = val;
	if (q->rear == NULL)
	{
		new_node->next = new_node;
		q->rear = new_node;
	}
	else
	{
		new_node->next = q->rear->next;
		q->rear->next = new_node;
		q->rear = new_node;
	}
	q->size++;
	return 0;
}
int chain_queue_pop(chain_queue* q)
{
	if (q == NULL)
	{
		return -1;
	}
	if (q->rear == NULL)
	{
		return -1;
	}
	if (q->rear->next == q->rear)
	{
		free(q->rear);
		q->rear = NULL;
	}
	else
	{
		chain_queue_node* abandoned = q->rear->next;
		q->rear->next = q->rear->next->next;
		free(abandoned);
	}
	q->size--;
	return 0;
}
size_t chain_queue_size(chain_queue* q)
{
	return q->size;
}
tree_node* chain_queue_front(chain_queue* q)
{
	assert(q != NULL && q->rear != NULL);
	return q->rear->next->val;
}
char chain_queue_empty(chain_queue* q)
{
	return q->size == 0;
}
void chain_queue_free(chain_queue* q)
{
	if (q == NULL)
	{
		return;
	}
	if (q->rear == NULL)
	{
		free(q);
		return;
	}
	chain_queue_node* node = q->rear->next;
	while (node != q->rear)
	{
		chain_queue_node* next = node->next;
		free(node);
		node = next;
	}
	free(q->rear);
	q->rear = NULL;
	free(q);
}
size_t tree_size(tree_node* root)
{
	if (root == NULL)
	{
		return 0;
	}
	size_t left = tree_size(root->left);
	size_t right = tree_size(root->right);
	return left + right + 1;
}
size_t tree_serialise_size(tree_node* root)
{
	chain_queue* q = chain_queue_init();
	chain_queue_push(q, root);
	size_t n = 0;
	while (!chain_queue_empty(q))
	{
		tree_node* current = chain_queue_front(q);
		if (current != NULL)
		{
			chain_queue_push(q, current->left);
			chain_queue_push(q, current->right);
		}
		chain_queue_pop(q);
		n++;
	}
	chain_queue_free(q);
	return n;
}
info* tree_serialise(tree_node* root,size_t n)
{
	info* ret = (info*)malloc(sizeof(info) * n);
	const info null_info = { -1,-1 };
	if (ret == NULL)
	{
		return NULL;
	}
	chain_queue *q=chain_queue_init();
	chain_queue_push(q, root);
	size_t i = 0;
	while (!chain_queue_empty(q))
	{
		tree_node* current = chain_queue_front(q);
		if (current != NULL)
		{
			chain_queue_push(q, current->left);
			chain_queue_push(q, current->right);
		}
		chain_queue_pop(q);
		ret[i++] = current==NULL?null_info:current->val;
	}
	chain_queue_free(q);
	return ret;
}
tree_node* tree_deserialise(info *val,size_t n)
{
	chain_queue* q = chain_queue_init();
	tree_node* root = tree_node_init(val[0], NULL, NULL);
	chain_queue_push(q, root);
	tree_node* current = root;
	size_t i = 1;
	char is_right = 0;
	while (i < n)
	{
		current = chain_queue_front(q);
		info current_val = val[i];
		if (current_val.weight != -1)
		{
			tree_node* new_node = tree_node_init(current_val, NULL, NULL);
			chain_queue_push(q,new_node);
			if (is_right)
			{
				current->right = new_node;
			}
			else
			{
				current->left = new_node;
			}
		}
		if (is_right)
		{
			chain_queue_pop(q);
		}
		is_right ^= 1;
		i++;
	}
	chain_queue_free(q);
	return root;
}

void tree_recursive_free(tree_node *root)
{
	if (root == NULL)
	{
		return;
	}
	tree_recursive_free(root->left);
	tree_recursive_free(root->right);
	free(root);
}
tree_node* huffman_tree_init(info *val,size_t n)
{
	tree_node** tree = (tree_node**)malloc(sizeof(tree_node*) * n);
	for (int i = 0; i < n; i++)
	{
		tree[i]=tree_node_init(val[i],NULL,NULL);
	}
	for (int i = 0; i + 1< n; i++)
	{
		int mn1 = -1;
		int mn2 = -1;
		for (int i = 0; i < n; i++)
		{
			if (tree[i] == NULL)
			{
				continue;
			}
			if (mn1==-1)
			{
				mn1 = i;
				continue;
			}
			else if (mn2 == -1)
			{
				mn2 = i;
			}
			if (tree[i]->val.weight < tree[mn1]->val.weight)
			{
				mn2 = mn1;
				mn1 = i;
			}
			else if (tree[i]->val.weight < tree[mn2]->val.weight)
			{
				mn2 = i;
			}
		}
		info new_info;
		new_info.character = -1;
		new_info.weight = tree[mn1]->val.weight + tree[mn2]->val.weight;
		tree[mn1]=tree_node_init(new_info, tree[mn1], tree[mn2]);
		tree[mn2] = NULL;
	}
	tree_node* ret=NULL;
	for (int i = 0; i < n; i++)
	{
		if (tree[i] != NULL)
		{
			ret = tree[i];
		}
	}
	free(tree);
	return ret;
}

void huffman_traverse(tree_node* root, char(*code)[128], char* current, int index)
{
	if (root == NULL)
	{
		return;
	}
	if (root->val.character != -1)
	{
		char temp = current[index];
		current[index] = 0;
		strcpy(code[root->val.character], current);
		current[index] = temp;
	}
	current[index] = '0';
	huffman_traverse(root->left, code, current,index+1);
	current[index] = '1';
	huffman_traverse(root->right, code, current ,index+1);
}
long filesize(FILE* fp)
{
	fpos_t fpos;
	fgetpos(fp, &fpos);
	fseek(fp, 0, SEEK_END);
	long ret = ftell(fp);
	fsetpos(fp, &fpos);
	return ret;
}
int main()
{
	tree_node* t=NULL;
	char code[128][128] = { "" };
	while(1)
	{
		printf("1 -- Initialize the Huffman Tree 2 -- Encode 3 -- Decode 4 -- Exit\nEnter:");
		char op = getchar();
		while (getchar() != '\n');
		switch (op)
		{
		case '1':
		{
			FILE* fp = fopen("hfmTree", "rb");
			if (fp != NULL)
			{
				printf("Overwrite hfmTree?(enter y to confirm)\n");
				if (getchar() != 'y')
				{
					break;
				}
				fclose(fp);
			}
			size_t n;
			printf("Enter the size of the character set:");
			scanf("%llu", &n);
			while (getchar() != '\n');
			info* val = (info*)malloc(sizeof(info) * n);
			for (int i = 0; i < n; i++)
			{
				scanf("%c %d", &val[i].character, &val[i].weight);
				while (getchar() != '\n');
			}
			t = huffman_tree_init(val, n);
			free(val);
			size_t size = tree_serialise_size(t);
			info* data = tree_serialise(t, size);
			fp = fopen("hfmTree", "wb");
			fwrite(data, sizeof(info), size, fp);
			fclose(fp);
			printf("Initialise successfully, saved in hfmTree\n");
			break;
		}
		case '2':
		{
			FILE* fp = fopen("hfmTree", "rb");
			if (fp == NULL)
			{
				printf("Failed to open hfmTree\n");
				exit(-1);
			}
			size_t n = filesize(fp) / sizeof(info);
			info* val = (info*)malloc(n * sizeof(info));
			fread(val, sizeof(info), n, fp);
			t = tree_deserialise(val, n);
			free(val);
			fclose(fp);
			char current[128];
			huffman_traverse(t, code, current, 0);
			FILE* f_text = fopen("TextFile", "rb");
			if (f_text == NULL)
			{
				printf("Failed to open TextFile\n");
				exit(-1);
			}
			FILE* f_code = fopen("CodeFile", "wb");
			char buffer[1024] = { 0 };
			char encoded[1024] = { 0 };
			size_t bytes = 0;
			//规定最后一个比特表示倒数第二个比特的有效信息位，其余位0填充
			int index = 0;
			char delta = 0;
			while ((bytes = fread(buffer, sizeof(char), 1024, f_text)) != 0)
			{
				for (int i = 0; i < bytes; i++)
				{
					char* p = code[buffer[i]];
					while (*p)
					{
						encoded[index] |= (*p == '1') ? (1 << delta) : (0);
						p++;
						delta++;
						if (delta == 8)
						{
							delta = 0;
							index++;
							if (index == 1024)
							{
								fwrite(encoded, sizeof(char), 1024, f_code);
								index = 0;
							}
						}
					}
				}
			}
			if (delta == 0)
			{
				fwrite(encoded, sizeof(char), index, f_code);
				delta = 8;
				fwrite(&delta, sizeof(char), 1, f_code);
			}
			else
			{
				fwrite(encoded, sizeof(char), index + 1, f_code);
				fwrite(&delta, sizeof(char), 1, f_code);
			}
			fclose(f_text);
			fclose(f_code);
			printf("Encoded successfully, saved in CodeFile\n");
			break;
		}
		case '3':
		{
			FILE* fp = fopen("hfmTree", "rb");
			if (fp == NULL)
			{
				printf("Failed to open hfmTree\n");
				exit(-1);
			}
			size_t n = filesize(fp) / sizeof(info);
			info* val = (info*)malloc(n * sizeof(info));
			fread(val, sizeof(info), n, fp);
			t = tree_deserialise(val, n);
			free(val);
			fclose(fp);
			FILE* f_code = fopen("CodeFile", "rb");
			if (f_code == NULL)
			{
				printf("Failed to open CodeFile\n");
				exit(-1);
			}
			FILE* f_text = fopen("TextFile", "wb");
			char buffer[128] = { 0 };
			char decoded[1024] = { 0 };
			size_t bytes = 0;
			//规定最后一个比特表示倒数第二个比特的有效信息位，其余位0填充
			int index = 0;
			char delta = 0;
			size_t size = filesize(f_code);
			tree_node* current = t;
			while (!feof(f_code))
			{
				bytes = fread(buffer, sizeof(char), 128, f_code);
				if (feof(f_code))
				{
					bytes -= 2;
				}
				else if (ftell(f_code) + 1 == size)
				{
					bytes--;
				}
				int i = 0;
				while (i < bytes)
				{
					current = t;
					while (i<bytes && current->val.character == -1)
					{
						if (buffer[i] & (1 << delta))
						{
							current = current->right;
						}
						else
						{
							current = current->left;
						}
						delta++;
						if (delta == 8)
						{
							delta = 0;
							i++;
						}
					}
					if (current->val.character != -1)
					{
						decoded[index] = current->val.character;
						index++;
						if (index == 1024)
						{
							fwrite(decoded, sizeof(char), 1024, f_text);
							index = 0;
						}
					}
				}
			}
			fwrite(decoded, sizeof(char), index, f_text);
			if (feof(f_code))
			{
				index = 0;
				while (delta < buffer[bytes+1])
				{
					if (current->val.character != -1)
					{
						current = t;
					}
					while (current->val.character == -1)
					{
						if (buffer[bytes] & (1 << delta))
						{
							current = current->right;
						}
						else
						{
							current = current->left;
						}
						delta++;
					}
					decoded[index] = current->val.character;
					index++;
				}
			}
			else
			{
				fread(buffer, sizeof(char), 2, f_code);
				index = 0;
				while (delta < buffer[1])
				{
					if (current->val.character != -1)
					{
						current = t;
					}
					while (current->val.character == -1)
					{
						if (buffer[0] & (1 << delta))
						{
							current = current->right;
						}
						else
						{
							current = current->left;
						}
						delta++;
					}
					decoded[index] = current->val.character;
					index++;
				}
			}
			fwrite(decoded, sizeof(char), index, f_text);
			fclose(f_code);
			fclose(f_text);
			printf("Decoded successfully, saved in TextFile\n");
			break;
		}
		case '4':
		{
			tree_recursive_free(t);
			exit(-1);
			break;
		}
		default:
		{
			break;
		}
		}
	}
	system("pause");
	return 0;
}