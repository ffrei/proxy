#include "adBlock.h"

#define DIE(msg, ...) do { fprintf(stderr, msg, ##__VA_ARGS__); exit(EXIT_FAILURE); } while(0)

char* exitString;
int exitStringSize;
char* classKey = "class";
char* addValue = "onf-ad";

static bool filter_node(myhtml_tree_node_t* node)
{
    assert(node);
    myhtml_tag_id_t tag = myhtml_node_tag_id(node);
    return (tag != MyHTML_TAG__TEXT) && (tag != MyHTML_TAG__END_OF_FILE) && (tag != MyHTML_TAG__COMMENT) && (tag != MyHTML_TAG__UNDEF);
}

static bool compare(char* value)
{
  FILE * fp;
  char * line = NULL;
  size_t len = 0;
  ssize_t read;
  char* valueChar;
  strcpy(valueChar, value);
  fp = fopen("easylist.txt", "r");
  if (fp == NULL){
    exit(EXIT_FAILURE);
  }
  while ((read = getline(&line, &len, fp)) != -1) {
      if (strstr(line, valueChar)) {
        printf("J'ai trouvé un point commun !!!\n");
        return true;
      }
  }
  fclose(fp);
  if (line) {
    free(line);
  }
  return false;
}

static void walk_subtree(myhtml_tree_t* tree, myhtml_tree_node_t* root, int level)
{
    if (!root) {
        printf("pas de root");
        return;
    }

    /* Check if we handle this node type */
    if (!filter_node(root)) {
        //printf("On ne gère pas les noeuds : %s \n", myhtml_tag_name_by_id(tree, myhtml_node_tag_id(root), NULL));
        return;
    }

    //printf("%s ", myhtml_tag_name_by_id(tree, myhtml_node_tag_id(root), NULL));
    myhtml_tree_attr_t* attr = myhtml_node_attribute_first(root);
    while (attr != NULL) {
        /* attribute sexpr (name value)*/
        const char *key = myhtml_attribute_key(attr, NULL);
        const char *value = myhtml_attribute_value(attr, NULL);

        if ((key!=NULL) && (value!=NULL)) {
          if((strcmp(key, classKey)==0) && compare(value)) {
            myhtml_node_delete_recursive(root);
            root = NULL;
          }
        }
        attr = myhtml_attribute_next(attr);
    }

    /* left hand depth-first recoursion */
    if (root != NULL){
      myhtml_tree_node_t* child = myhtml_node_child(root);
      while (child != NULL) {
          myhtml_tree_node_t* nextChild = myhtml_node_next(child);
          walk_subtree(tree, child, level + 1);
          child = nextChild;
      }
    }


    /* close sexpr */
    //printf("\n");
    //putchar(')');
}

struct res_html {
    char  *html;
    size_t size;
};

mystatus_t write_output(const char* data, size_t len, void* ctx){


	char* exitString_tmp = (char*)realloc(exitString,(exitStringSize+len)* sizeof(char));
  if(exitString_tmp==NULL){
    printf("The re-allocation of array a has failed");
  }else{
    exitString=exitString_tmp;
    int i;
		for(i=0;i<len;i++){
      exitString[exitStringSize+i]=data[i];
    }
	}

  exitStringSize+=len;
  return MyCORE_STATUS_OK;
}

char* cleanAd(char* startChar, size_t size) {
  exitString=malloc(1);
  exitString[0]=' ';
  exitStringSize=1;
  struct res_html data= {startChar, size};
  mystatus_t res;

	// basic init
  myhtml_t* myhtml = myhtml_create();
  if (!myhtml) {
    DIE("myhtml_create failed\n");
  }

  res = myhtml_init(myhtml, MyHTML_OPTIONS_DEFAULT, 1, 0);
  if (MYHTML_FAILED(res)) {
  	DIE("myhtml_init failed with %d\n", res);
  }

  // init tree
  myhtml_tree_t* tree = myhtml_tree_create();
  if (!tree) {
    DIE("myhtml_tree_create failed\n");
  }

  res = myhtml_tree_init(tree, myhtml);
  if (MYHTML_FAILED(res)) {
  	DIE("myhtml_tree_init failed with %d\n", res);
  }

  // parse html
  myhtml_parse(tree, MyENCODING_UTF_8, data.html, data.size);
  walk_subtree(tree, myhtml_tree_get_node_html(tree), 0);
  myhtml_serialization_tree_callback(myhtml_tree_get_document(tree), write_output, NULL);

  // release resources
  myhtml_tree_destroy(tree);
  myhtml_destroy(myhtml);

  return exitString;
}
