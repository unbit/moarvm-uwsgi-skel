#include <uwsgi.h>
#include <moar.h>

struct moarvm_config {
	MVMInstance *main;
	struct uwsgi_string_list *lib_path;
	char *load;
} moarvm;

static struct uwsgi_option moarvm_options[] = {
	{"moarvm-libpath", required_argument, 0, "add the specified libpath to the moarvm engine", uwsgi_opt_add_string_list, &moarvm.lib_path, 0},
	{"moarvm-load", required_argument, 0, "load the specified code in the moarvm engine", uwsgi_opt_set_str, &moarvm.load, 0},
	UWSGI_END_OF_OPTIONS
};

static int moarvm_init() {
	moarvm.main = MVM_vm_create_instance();
	struct uwsgi_string_list *usl = NULL;
	int count = 0;
	uwsgi_foreach(usl, moarvm.lib_path) {
		if (count >= 8) {
			uwsgi_log("you can specify a max of 8 moarvm libpath\n");
			exit(1);
		}
		moarvm.main->lib_path[count] = usl->value;
		count++;
	}
	moarvm.main->lib_path[count] = NULL;	
	moarvm.main->num_clargs = 1;
	char *tmp_args[2];
	tmp_args[0] = "hello.pl";
	tmp_args[1] = NULL;
	moarvm.main->raw_clargs = tmp_args;
	moarvm.main->prog_name = moarvm.load;
	uwsgi_log("I Am MoarVM at %p\n");
	if (!moarvm.load) return 0;

	MVM_vm_run_file(moarvm.main, moarvm.load);
	uwsgi_log("%s loaded\n", moarvm.load);

	MVMThreadContext *tc = moarvm.main->main_thread;
	MVMCompUnit      *cu = MVM_cu_map_from_file(tc, moarvm.load);
	uwsgi_log("cu = %p %p\n", cu, tc->cur_frame);
	uint32_t i;
	for(i=0;i<cu->body.num_frames;i++) {
		uwsgi_log("frame %s\n", MVM_string_utf8_encode_C_string(tc, cu->body.frames[i]->body.name));
	}
	return 0;
}

struct uwsgi_plugin moarvm_plugin = {
	// the perl-assigned modifier1
	.modifier1 = 5,
	.name =	"moarvm",
	.options = moarvm_options,
	.init = moarvm_init,
};
