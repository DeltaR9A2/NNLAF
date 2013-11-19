#include "anim.h"

fset_t *fset_create(const char *fn, const wchar_t *name, int cols, int rows){
    fset_t *frameset = malloc(sizeof(fset_t));

    wcsncpy(frameset->name, name, NAME_MAX_LEN-1);
    frameset->next = NULL;
    
    frameset->len = cols * rows;
    frameset->frames = malloc(sizeof(SDL_Surface*)*(frameset->len));

    SDL_Surface *tmp = load_image(fn);
    SDL_Surface *frame = NULL;

    SDL_Rect frame_rect;
    frame_rect.x = 0;
    frame_rect.y = 0;
    frame_rect.w = tmp->w / cols;
    frame_rect.h = tmp->h / rows;
    
    for(int r=0; r<rows; r++){
        for(int c=0; c<cols; c++){ // lol c++ >_>
            frame = create_surface(frame_rect.w, frame_rect.h);
            frame_rect.x = c * frame_rect.w;
            frame_rect.y = r * frame_rect.h;
            SDL_BlitSurface(tmp, &frame_rect, frame, NULL);
            frameset->frames[c+(r*cols)] = frame;
        }
    }
    
    SDL_FreeSurface(tmp);
    
    return frameset;
}

void fset_delete(fset_t *frameset){
    for(int i=0; i<frameset->len; i++){
        SDL_FreeSurface(frameset->frames[i]);
    }
    free(frameset->frames);
    free(frameset);
}

void fset_list_add(fset_t *head, fset_t *frameset){
    fset_t *iter = head;
    
    while(iter->next != NULL){
        if(iter == frameset){
            return;
        }else{
            iter = iter->next;
        }
    }
    
    iter->next = frameset;
}

fset_t *fset_list_get(fset_t *head, const wchar_t *name){
    fset_t *iter = head;
    
    while(wcscmp(name, iter->name) != 0){
        if(iter->next == NULL){
            return NULL;
        }else{
            iter = iter->next;
        }
    }
    
    return iter;
}

void fset_list_delete(fset_t *head){
    fset_t *iter = head;
    fset_t *next = NULL;

    while(iter != NULL){
        next = iter->next;
        fset_delete(iter);
        iter = next;
    }
}

anim_t *anim_create(const wchar_t *name, int len, int fps){
    anim_t *anim = malloc(sizeof(anim_t));

    wcsncpy(anim->name, name, NAME_MAX_LEN-1);
    anim->next = NULL;
    anim->len = len;
    anim->fps = fps;

    anim->frames = malloc(sizeof(SDL_Surface*)*(anim->len));
    for(int i=0; i < anim->len; i++){
        anim->frames[i] = NULL;
    }

    return anim;
}

void anim_delete(anim_t *anim){
    free(anim->frames);
    free(anim);
}

void anim_list_add(anim_t *head, anim_t *anim){
    anim_t *iter = head;
    
    while(iter->next != NULL){
        if(iter == anim){
            return;
        }else{
            iter = iter->next;
        }
    }
    
    iter->next = anim;
}

anim_t *anim_list_get(anim_t *head, const wchar_t *name){
    anim_t *iter = head;
    
    while(wcscmp(name, iter->name) != 0){
        if(iter->next == NULL){
            return NULL;
        }else{
            iter = iter->next;
        }
    }
    
    return iter;
}

void anim_list_delete(anim_t *head){
    anim_t *iter = head;
    anim_t *next = NULL;

    while(iter != NULL){
        next = iter->next;
        anim_delete(iter);
        iter = next;
    }
}

void anim_add_frame(anim_t *anim, fset_t *frameset, int frame_index){
    for(int i=0; i < anim->len; i++){
        if(anim->frames[i] == NULL){
            anim->frames[i] = frameset->frames[frame_index];
            return;
        }
    }
    fprintf(stderr,"Warning: anim_add_frame: No more room for frame");
}

void anim_set_frames(anim_t *anim, fset_t *frameset, int start_index){
    for(int i=0; i < anim->len; i++){
        anim->frames[i] = frameset->frames[start_index + i];
    }
}    

void anim_draw(anim_t *anim, int step, SDL_Surface *target, SDL_Rect *dest){
    // Magic number 0.01666 is the inverse of 60
    // and 60 is the refresh rate of the main loop
    SDL_BlitSurface(anim->frames[((int)((step*0.01666) * anim->fps)) % anim->len], NULL, target, dest);
}

