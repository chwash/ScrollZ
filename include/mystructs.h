#ifndef _mystructs_h_
#define _mystructs_h_

/*
 * Structures for my use
 *
 * $Id: mystructs.h,v 1.1.1.1 1998-09-10 17:31:12 f Exp $
 */

/* don't change!! */
#define mybufsize     1024
/* don't change!! */
#define HASHTABLESIZE 31
/* change to number of autoreply nicks */
#define AUTOREPLYSIZE 5

/* don't change!! */
#ifdef WANTANSI
#define NUMCMDCOLORS_  25

#if defined(CELECOSM) && defined(OPERVISION)
#define NUMCMDCOLORS	(NUMCMDCOLORS_ + 2)
#elif defined(CELECOSM) || defined(OPERVISION)
#define NUMCMDCOLORS	(NUMCMDCOLORS_ + 1)
#else  /* CELECOSM || OPERVISION */
#define NUMCMDCOLORS	(NUMCMDCOLORS_)
#endif /* CELECOSM && OPERVISION */

#define NUMCOLORS     22
#endif /* WANTANSI */

/* some defines for friends list */
#define FLINVITE 1
#define FLCHOPS 2
#define FLOP 4
#define FLAUTOOP 8
#define FLUNBAN 16
#define FLPROT 32
#define FLCDCC 64
#define FLGOD 128
#define FLVOICE 256
#define FLJOIN 512
#define FLNOFLOOD 1024
#define FLALL 2047

/* some defines for shit list */
#define SLKICK 1
#define SLBAN 2
#define SLIGNORE 4
#define SLPERMBAN 8
#define SLDEOP 16

struct friends {
     struct friends *next;
     char *userhost;
     char *channels;
     char *passwd;
     int privs;
     int number;
};

struct autobankicks {
     struct autobankicks *next;
     char *userhost;
     char *reason;
     char *channels;
     int shit;
};

struct list {
     struct list *next;
     char *nick;
     char *userhost;
};

struct words {
     struct words *next;
     char *channels;
     char *word;
     char *reason;
};

struct nicks {
     struct nicks *next;
     char *nick;
};

struct wholeftch {
   struct wholeftch *next;
   char *channel;
   struct list *nicklist;
};

struct wholeftstr {
   struct wholeftstr *next;
   char *splitserver;
   int print;
   int count;
   int total;
   time_t time;
   struct wholeftch *channels;
};

struct splitstr {
    struct splitstr *next;
    char *servers;
};

struct urlstr {
    struct urlstr *next;
    char *urls;
};

struct mapstr {
    struct mapstr *next;
    char *server;
    char *uplink;
    int  distance;
};

struct bans {
    struct bans *next;
    char *ban;
    char *who;
    time_t when;
};

struct spingstr {
    struct spingstr *next;
    char *servername;
    long sec;
    long usec;
};

#ifdef WANTANSI
struct colorstr {
    char *color1;
    char *color2;
    char *color3;
    char *color4;
    char *color5;
    char *color6;
};
#endif

#endif /* _mystructs_h_ */
