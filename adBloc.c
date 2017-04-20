#include "adBloc.h"

#define DIE(msg, ...) do { fprintf(stderr, msg, ##__VA_ARGS__); exit(EXIT_FAILURE); } while(0)

char* exitString;

char* classKey = "class";
char* addValue = "onf-ad";

static bool filter_node(myhtml_tree_node_t* node)
{
    assert(node);
    myhtml_tag_id_t tag = myhtml_node_tag_id(node);
    return (tag != MyHTML_TAG__TEXT) && (tag != MyHTML_TAG__END_OF_FILE) && (tag != MyHTML_TAG__COMMENT) && (tag != MyHTML_TAG__UNDEF);
}

/* depth-first lefthand tree walk */
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

    /* start sexpr */
    //putchar('(');

    /* print this node */
    //printf("%s ", myhtml_tag_name_by_id(tree, myhtml_node_tag_id(root), NULL));
    myhtml_tree_attr_t* attr = myhtml_node_attribute_first(root);
    while (attr != NULL) {
        /* attribute sexpr (name value)*/
        const char *key = myhtml_attribute_key(attr, NULL);
        const char *value = myhtml_attribute_value(attr, NULL);

        if ((key!=NULL) && (value!=NULL)) {
          if((strcmp(key, classKey)==0) && (strcmp(value, addValue)==0)) {
            myhtml_node_delete_recursive(root);
            root = NULL;
          }
        }
        /*if (key==NULL)
            printf("(KEY IS NULL)");
        else if (value)
            printf("(%s \'%s\')", key, value);
        else
            printf("(%s)", key);*/

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

struct res_html load_html_file(const char* filename)
{
    FILE *fh = fopen(filename, "rb");
    if(fh == NULL) {
        DIE("Can't open html file: %s\n", filename);
    }

    if(fseek(fh, 0L, SEEK_END) != 0) {
        DIE("Can't set position (fseek) in file: %s\n", filename);
    }

    long size = ftell(fh);

    if(fseek(fh, 0L, SEEK_SET) != 0) {
        DIE("Can't set position (fseek) in file: %s\n", filename);
    }

    if(size <= 0) {
        fclose(fh);

        struct res_html res = {NULL, 0};
        return res;
    }

    char *html = (char*)malloc(size + 1);
    if(html == NULL) {
        DIE("Can't allocate mem for html file: %s\n", filename);
    }

    size_t nread = fread(html, 1, size, fh);
    if (nread != size) {
        DIE("could not read %ld bytes (" MyCORE_FMT_Z " bytes done)\n", size, nread);
    }

    fclose(fh);

    struct res_html res = {html, (size_t)size};
    return res;
}

static void usage(void)
{
    fprintf(stderr, "html2sexpr <file>\n");
}

mystatus_t write_output(const char* data, size_t len, void* ctx)
{
	exitString = malloc(strlen(data)+1);
	if (exitString) {
		strcpy(exitString, data);
		//printf("%s\n", exitString);
	}	
    return MyCORE_STATUS_OK;
}

char* cleanAd(struct res_html data) {
  
  //struct res_html data = load_html_file(entry); //changement de fonction à realiser
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
  printf("\n");
  myhtml_serialization_tree_callback(myhtml_tree_get_document(tree), write_output, NULL);
  // release resources
  myhtml_tree_destroy(tree);
  myhtml_destroy(myhtml);
  return data.html;
}

int main(int argc,char *argv[]){
	if (argc != 2) {
	    usage();
	    DIE("Invalid number of arguments\n");	
	}
	char* startChar = "<!DOCTYPE html><html><body><p > bonjour</p><script>document.innerHTML;<script></body></html>";
	struct res_html data = {startChar, (size_t)strlen(startChar)};	
	char* endChar = cleanAd(data);
	printf("%s\n", endChar);
	return 0;

}

