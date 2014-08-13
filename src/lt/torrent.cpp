#include "deps.hpp"

#define MEMBER(a)	a
#define STRUCT_MEMBER(s,m)	s. MEMBER(m)
#define STRUCTP_MEMBER(s,m)	s-> MEMBER(m)
#define TSTATE_ASSIGN(a,b,m,t)	\
	STRUCTP_MEMBER(a,m) = (t) STRUCT_MEMBER(b,m)

extern "C" {

#include "torrent.h"

/* @lt_trnt_status_cnvt(): convert a torrent status */
static void lt_trnt_status_cnvt(libtorrent::torrent_handle *,torrent_status *);

/* @lt_trnt_state_cnvt(): convert a torrent state */
static enum torrent_state
	lt_trnt_state_cnvt(enum libtorrent::torrent_status::state_t);

torrent_params *lt_trnt_params_create(void) {
	return reinterpret_cast<torrent_params*>
		(new libtorrent::add_torrent_params());
}

void lt_trnt_params_destroy(torrent_params *tp) {
	delete reinterpret_cast<libtorrent::add_torrent_params*>(tp);
}

void lt_trnt_params_set_savepath(torrent_params *tp, const char *path) {
	libtorrent::add_torrent_params *torp
		= reinterpret_cast<libtorrent::add_torrent_params*>(tp);
	torp->save_path = std::string(path);
}

torrent_info *lt_trnt_info_create(const char *path) {
	try {
		return reinterpret_cast<torrent_info*>
			(new libtorrent::torrent_info(std::string(path)));
	} catch (libtorrent::libtorrent_exception& e) {
		return NULL;
	}
}

void lt_trnt_info_destroy(torrent_info *ti) {
	delete reinterpret_cast<libtorrent::torrent_info*>(ti);
}

void lt_trnt_params_set_info(torrent_params *tp, torrent_info *ti) {
	libtorrent::add_torrent_params *torp
		= reinterpret_cast<libtorrent::add_torrent_params*>(tp);
	torp->ti = reinterpret_cast<libtorrent::torrent_info*>(ti);
}

void lt_trnt_params_set_url(torrent_params *tp, const char *url) {
	libtorrent::add_torrent_params *torp
		= reinterpret_cast<libtorrent::add_torrent_params*>(tp);
	torp->url = std::string(url);
}

const char *lt_trnt_params_get_name(torrent_params *tp) {
	return reinterpret_cast<libtorrent::add_torrent_params*>
		(tp)->name.c_str();
}

void lt_trnt_params_set_flags(torrent_params *tp, uint64_t flags) {
	reinterpret_cast<libtorrent::add_torrent_params*>(tp)->flags = flags;
}

bool lt_trnt_handle_is_valid(torrent_handle *th) {
	return reinterpret_cast<libtorrent::torrent_handle*>(th)->is_valid();
}

void lt_trnt_handle_pause(torrent_handle *th) {
	reinterpret_cast<libtorrent::torrent_handle*>(th)->pause();
}

void lt_trnt_handle_resume(torrent_handle *th) {
	reinterpret_cast<libtorrent::torrent_handle*>(th)->resume();
}

void lt_trnt_handle_set_priority(torrent_handle *th, int prio) {
	reinterpret_cast<libtorrent::torrent_handle*>(th)->set_priority(prio);
}

void lt_trnt_handle_set_upload_limit(torrent_handle *th, int bytes) {
	reinterpret_cast<libtorrent::torrent_handle*>
		(th)->set_upload_limit(bytes);
}

int lt_trnt_handle_get_upload_limit(torrent_handle *th) {
	return reinterpret_cast<libtorrent::torrent_handle*>
		(th)->upload_limit();
}

void lt_trnt_handle_set_download_limit(torrent_handle *th, int bytes) {
	reinterpret_cast<libtorrent::torrent_handle*>
		(th)->set_download_limit(bytes);
}

int lt_trnt_handle_get_download_limit(torrent_handle *th) {
	return reinterpret_cast<libtorrent::torrent_handle*>
		(th)->download_limit();
}

void lt_trnt_handle_set_max_connections(torrent_handle *th, int max) {
	reinterpret_cast<libtorrent::torrent_handle*>
		(th)->set_max_connections(max);
}

void lt_trnt_handle_set_super_seeding(torrent_handle *th, bool v) {
	reinterpret_cast<libtorrent::torrent_handle*>
		(th)->super_seeding(v);
}

void lt_trnt_handle_destroy(torrent_handle *th) {
	delete reinterpret_cast<libtorrent::torrent_handle*>(th);
}

torrent_status *lt_trnt_handle_get_status(torrent_handle *th) {
	static torrent_status ts;

	lt_trnt_status_cnvt(reinterpret_cast<libtorrent::torrent_handle*>(th),
			    &ts);
	return &ts;
}

static void
lt_trnt_status_cnvt(libtorrent::torrent_handle *th, torrent_status *ts) {
	libtorrent::torrent_status tstat = 
		th->status(libtorrent::torrent_handle::query_name
			| libtorrent::torrent_handle::query_save_path);

	strncpy(ts->error, tstat.error.c_str(), LT_STATUS_LEN-1);
	ts->error[LT_STATUS_LEN-1] = '\0';
	strncpy(ts->save_path, tstat.save_path.c_str(), LT_STATUS_LEN-1);
	ts->save_path[LT_STATUS_DIR_LEN-1] = '\0';
	strncpy(ts->name, tstat.name.c_str(), LT_STATUS_LEN-1);
	ts->name[LT_STATUS_LEN-1] = '\0';
	TSTATE_ASSIGN(ts,tstat,total_download,uint64_t);
	TSTATE_ASSIGN(ts,tstat,total_upload,uint64_t);
	TSTATE_ASSIGN(ts,tstat,total_payload_download,uint64_t);
	TSTATE_ASSIGN(ts,tstat,total_payload_upload,uint64_t);
	TSTATE_ASSIGN(ts,tstat,total_redundant_bytes,uint64_t);
	TSTATE_ASSIGN(ts,tstat,total_done,uint64_t);
	TSTATE_ASSIGN(ts,tstat,total_wanted_done,uint64_t);
	TSTATE_ASSIGN(ts,tstat,total_wanted,uint64_t);
	TSTATE_ASSIGN(ts,tstat,all_time_upload,uint64_t);
	TSTATE_ASSIGN(ts,tstat,all_time_download,uint64_t);
	TSTATE_ASSIGN(ts,tstat,added_time,time_t);
	TSTATE_ASSIGN(ts,tstat,completed_time,time_t);
	TSTATE_ASSIGN(ts,tstat,last_seen_complete,time_t);
	TSTATE_ASSIGN(ts,tstat,progress,float);
	TSTATE_ASSIGN(ts,tstat,progress_ppm,int);
	TSTATE_ASSIGN(ts,tstat,queue_position,int);
	TSTATE_ASSIGN(ts,tstat,download_rate,int);
	TSTATE_ASSIGN(ts,tstat,upload_rate,int);
	TSTATE_ASSIGN(ts,tstat,download_payload_rate,int);
	TSTATE_ASSIGN(ts,tstat,upload_payload_rate,int);
	TSTATE_ASSIGN(ts,tstat,num_seeds,int);
	TSTATE_ASSIGN(ts,tstat,num_peers,int);
	TSTATE_ASSIGN(ts,tstat,num_complete,int);
	TSTATE_ASSIGN(ts,tstat,num_incomplete,int);
	TSTATE_ASSIGN(ts,tstat,list_seeds,int);
	TSTATE_ASSIGN(ts,tstat,connect_candidates,int);
	TSTATE_ASSIGN(ts,tstat,num_pieces,int);
	TSTATE_ASSIGN(ts,tstat,distributed_full_copies,int);
	TSTATE_ASSIGN(ts,tstat,distributed_fraction,int);
	TSTATE_ASSIGN(ts,tstat,distributed_copies,float);
	TSTATE_ASSIGN(ts,tstat,block_size,int);
	TSTATE_ASSIGN(ts,tstat,num_uploads,int);
	TSTATE_ASSIGN(ts,tstat,num_connections,int);
	TSTATE_ASSIGN(ts,tstat,uploads_limit,int);
	TSTATE_ASSIGN(ts,tstat,connections_limit,int);
	TSTATE_ASSIGN(ts,tstat,up_bandwidth_queue,int);
	TSTATE_ASSIGN(ts,tstat,down_bandwidth_queue,int);
	TSTATE_ASSIGN(ts,tstat,time_since_upload,int);
	TSTATE_ASSIGN(ts,tstat,time_since_download,int);
	TSTATE_ASSIGN(ts,tstat,active_time,int);
	TSTATE_ASSIGN(ts,tstat,finished_time,int);
	TSTATE_ASSIGN(ts,tstat,seeding_time,int);
	TSTATE_ASSIGN(ts,tstat,seed_rank,int);
	TSTATE_ASSIGN(ts,tstat,last_scrape,int);
	TSTATE_ASSIGN(ts,tstat,sparse_regions,int);
	TSTATE_ASSIGN(ts,tstat,priority,int);
	ts->state = lt_trnt_state_cnvt(tstat.state);
	TSTATE_ASSIGN(ts,tstat,need_save_resume,bool);
	TSTATE_ASSIGN(ts,tstat,ip_filter_applies,bool);
	TSTATE_ASSIGN(ts,tstat,upload_mode,bool);
	TSTATE_ASSIGN(ts,tstat,share_mode,bool);
	TSTATE_ASSIGN(ts,tstat,super_seeding,bool);
	TSTATE_ASSIGN(ts,tstat,paused,bool);
	TSTATE_ASSIGN(ts,tstat,auto_managed,bool);
	TSTATE_ASSIGN(ts,tstat,sequential_download,bool);
	TSTATE_ASSIGN(ts,tstat,is_seeding,bool);
	TSTATE_ASSIGN(ts,tstat,is_finished,bool);
	TSTATE_ASSIGN(ts,tstat,has_metadata,bool);
	TSTATE_ASSIGN(ts,tstat,seed_mode,bool);
	TSTATE_ASSIGN(ts,tstat,moving_storage,bool);
}

static enum torrent_state
lt_trnt_state_cnvt(enum libtorrent::torrent_status::state_t state) {
	switch (state) {
	case queued_for_checking:
		return (enum torrent_state) queued_for_checking;
	case checking_files:
		return (enum torrent_state) checking_files;
	case downloading_metadata:
		return (enum torrent_state) downloading_metadata;
	case downloading:
		return (enum torrent_state) downloading;
	case finished:
		return (enum torrent_state) finished;
	case seeding:
		return (enum torrent_state) seeding;
	case allocating:
		return (enum torrent_state) allocating;
	case checking_resume_data:
		return (enum torrent_state) checking_resume_data;
	default:
		return (enum torrent_state) unknown;
	}
}

torrent_handle *lt_trnt_vector_get(torrent_vector *tv, uint64_t i) {
	std::vector<libtorrent::torrent_handle> *hvec
	      = reinterpret_cast<std::vector<libtorrent::torrent_handle>*>(tv);
	if (i < hvec->size())
		return reinterpret_cast<torrent_handle*>(&hvec->at(i));
	else return NULL;
}

bool lt_trnt_handle_equal(torrent_handle *a, torrent_handle *b) {
	libtorrent::torrent_handle *tha, *thb;

	tha = reinterpret_cast<libtorrent::torrent_handle*>(a);
	thb = reinterpret_cast<libtorrent::torrent_handle*>(b);

	return *tha == *thb;
}

}
