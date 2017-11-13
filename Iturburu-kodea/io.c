#include "definitions.h"
#include "load_obj.h"
#include "io.h"
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>


extern object3d * _first_object;
extern object3d * _selected_object;
//extern aldaketa * aldaketak;

extern GLdouble _ortho_x_min,_ortho_x_max;
extern GLdouble _ortho_y_min,_ortho_y_max;
extern GLdouble _ortho_z_min,_ortho_z_max;

int egoera;
int erreferentzia = 2;

/**
 * @brief This function just prints information about the use
 * of the keys
 */
void print_help(){
    printf("KbG Irakasgaiaren Praktika. Programa honek 3D objektuak \n");
    printf("aldatzen eta bistaratzen ditu.  \n\n");

    printf("Egilea: Borja Calvo (borja.calvo@ehu.es) \n");
    printf("Data: Irailak, 2014 \n");
    printf("\n\n");
    printf("FUNTZIO NAGUSIAK \n");
    printf("<?>\t\t Laguntza hau bistaratu \n");
    printf("<ESC>\t\t Programatik irten \n");
    printf("<F>\t\t Objektua bat kargatu\n");
    printf("<B>\t\t Biraketa aktibatu\n");
    printf("<M>\t\t Traslazioa aktibatu\n");
    printf("<T>\t\t Tamaina aldaketa aktibatu\n");
    printf("<G>\t\t Aldaketak munduaren erreferentzi sisteman eragin (aldaketa globalak, alegia)\n");
    printf("<L>\t\t Aldaketak objektuaren (edo kameraren, edo argiaren) erreferentzi sisteman eragin (aldaketa lokalak, alegia)\n");
    printf("<O>\t\t Aldaketak hautaturik dagoen objektuari eragin\n");
    printf("<TAB>\t\t Kargaturiko objektuen artean bat hautatu\n");
    printf("<DEL>\t\t Hautatutako objektua ezabatu\n");
    printf("<CTRL + ->\t Bistaratze-eremua handitu\n");
    printf("<CTRL + +>\t Bistaratze-eremua txikitu\n");
    printf("<CTRL + Z>\t Aldaketak desegin\n");
    printf("<GORA>\t\t Mugitu +Y; Txikitu Y; Biratu +X\n");
    printf("<BEHERA>\t Mugitu -Y; Handitu Y; Biratu -X\n");
    printf("<ESKUINA>\t Mugitu +X; Txikitu X; Biratu +Y\n");
    printf("<EZKERRA>\t Mugitu -X; Handitu X; Biratu -Y\n");
    printf("<AVPAG>\t\t Mugitu +Z; Txikitu Z; Biratu +Z\n");
    printf("<REPAG>\t\t Mugitu -Z; Handitu Z; Biratu -Z\n");
    printf("<+>\t\t Ardatz guztietan handitu (Objektuak bakarrik)\n");
    printf("<->\t\t Ardatz guztietan txikitu (Objektuak bakarrik)\n");
    printf("\n\n");
}

/**
 * @brief Callback function to control the basic keys
 * @param key Key that has been pressed
 * @param x X coordinate of the mouse pointer when the key was pressed
 * @param y Y coordinate of the mouse pointer when the key was pressed
 */
void keyboard(unsigned char key, int x, int y) {

    char* fname = malloc(sizeof (char)*128); /* Note that scanf adds a null character at the end of the vector*/
    int read = 0;
    object3d *auxiliar_object = 0;
    GLdouble wd,he,midx,midy;
    GLdouble * matrix = malloc(sizeof(GLdouble)*16);
    

    switch (key) {
        case 'f':
        case 'F':
            /*Ask for file*/
            printf("%s", KG_MSSG_SELECT_FILE);
            scanf("%s", fname);
            /*Allocate memory for the structure and read the file*/
            auxiliar_object = (object3d *) malloc(sizeof (object3d));
            read = read_wavefront(fname, auxiliar_object);
            switch (read) {
            /*Errors in the reading*/
            case 1:
                printf("%s: %s\n", fname, KG_MSSG_FILENOTFOUND);
                break;
            case 2:
                printf("%s: %s\n", fname, KG_MSSG_INVALIDFILE);
                break;
            case 3:
                printf("%s: %s\n", fname, KG_MSSG_EMPTYFILE);
                break;
            /*Read OK*/
            case 0:
                /*Insert the new object in the list*/
                auxiliar_object->next = _first_object;
                _first_object = auxiliar_object;
                _selected_object = _first_object;
                printf("%s\n",KG_MSSG_FILEREAD);
                break;
            }
            break;

        case 9: /* <TAB> */
            if (_selected_object != NULL){
    		_selected_object = _selected_object->next;
            	/*The selection is circular, thus if we move out of the list we go back to the first element*/
            	if (_selected_object == 0) _selected_object = _first_object;
    	}
            break;

        case 127: /* <SUPR> */
    	if( _selected_object != NULL){
            /*Erasing an object depends on whether it is the first one or not*/
            	if (_selected_object == _first_object)
            	{
                	/*To remove the first object we just set the first as the current's next*/
                	_first_object = _first_object->next;
                	/*Once updated the pointer to the first object it is save to free the memory*/
                	free(_selected_object);
                	/*Finally, set the selected to the new first one*/
                	_selected_object = _first_object;
            	} else {
                	/*In this case we need to get the previous element to the one we want to erase*/
                	auxiliar_object = _first_object;
                	while (auxiliar_object->next != _selected_object)
                    	auxiliar_object = auxiliar_object->next;
                		/*Now we bypass the element to erase*/
                		auxiliar_object->next = _selected_object->next;
                		/*free the memory*/
                		free(_selected_object);
                		/*and update the selection*/
                		_selected_object = auxiliar_object;
            	}
    	}
            break;

        case '-':
			if(_selected_object != NULL){
				matrix[0] = 0.8;
				matrix[1] = 0;
				matrix[2] = 0;
				matrix[3] = 0;
				matrix[4] = 0;
				matrix[5] = 0.8;
				matrix[6] = 0;
				matrix[7] = 0;
				matrix[8] = 0;
				matrix[9] = 0;
				matrix[10] = 0.8;
				matrix[11] = 0;
				matrix[12] = 0;
				matrix[13] = 0;
				matrix[14] = 0;
				matrix[15] = 1;
				_selected_object->matrix = matrix_multiplication(_selected_object->matrix, matrix);
			}
            if (glutGetModifiers() == GLUT_ACTIVE_CTRL){
                /*Increase the projection plane; compute the new dimensions*/
                wd=(_ortho_x_max-_ortho_x_min)/KG_STEP_ZOOM;
                he=(_ortho_y_max-_ortho_y_min)/KG_STEP_ZOOM;
                /*In order to avoid moving the center of the plane, we get its coordinates*/
                midx = (_ortho_x_max+_ortho_x_min)/2;
                midy = (_ortho_y_max+_ortho_y_min)/2;
                /*The the new limits are set, keeping the center of the plane*/
                _ortho_x_max = midx + wd/2;
                _ortho_x_min = midx - wd/2;
                _ortho_y_max = midy + he/2;
                _ortho_y_min = midy - he/2;
            } else {

    	}
            break;

        case '+':
			if(_selected_object != NULL){
				matrix[0] = 1.25;
				matrix[1] = 0;
				matrix[2] = 0;
				matrix[3] = 0;
				matrix[4] = 0;
				matrix[5] = 1.25;
				matrix[6] = 0;
				matrix[7] = 0;
				matrix[8] = 0;
				matrix[9] = 0;
				matrix[10] = 1.25;
				matrix[11] = 0;
				matrix[12] = 0;
				matrix[13] = 0;
				matrix[14] = 0;
				matrix[15] = 1;
				_selected_object->matrix = matrix_multiplication(_selected_object->matrix, matrix);
				
			}
            if (glutGetModifiers() == GLUT_ACTIVE_CTRL){
                /*Increase the projection plane; compute the new dimensions*/
                wd=(_ortho_x_max-_ortho_x_min)*KG_STEP_ZOOM;
                he=(_ortho_y_max-_ortho_y_min)*KG_STEP_ZOOM;
                /*In order to avoid moving the center of the plane, we get its coordinates*/
                midx = (_ortho_x_max+_ortho_x_min)/2;
                midy = (_ortho_y_max+_ortho_y_min)/2;
                /*The the new limits are set, keeping the center of the plane*/
                _ortho_x_max = midx + wd/2;
                _ortho_x_min = midx - wd/2;
                _ortho_y_max = midy + he/2;
                _ortho_y_min = midy - he/2;
            } else {

    		}
            break;
    	
        case 'z':
        case 'Z':
    	if (glutGetModifiers() == GLUT_ACTIVE_CTRL){

    	}
    	break;

        case '?':
            print_help();
            break;

        case 27: /* <ESC> */
            exit(0);
            break;

    case 'm':
    case 'M':
        egoera = 1;
	break;

    case 'b':
    case 'B':
        egoera = 2;
	break;

    case 't':
    case 'T':
	   egoera = 3;
	break;

    case 'g':
    case 'G':
	   erreferentzia = 1;
	break;

    case 'l':
    case 'L':
	   erreferentzia = 2;
	break;

	case 'p':
	case 'P':
		if( _selected_object != NULL){
			GLdouble* m = malloc(sizeof(GLdouble)*16);
			m = _selected_object->matrix;
			printf("%f %f %f %f \n", m[0], m[4], m[8], m[12]);
			printf("%f %f %f %f \n", m[1], m[5], m[9], m[13]);
			printf("%f %f %f %f \n", m[2], m[6], m[10], m[14]);
			printf("%f %f %f %f \n", m[3], m[7], m[11], m[15]);
		}
	break;


    default:
        /*In the default case we just print the code of the key. This is usefull to define new cases*/
        printf("%d %c\n", key, key);
    }
    /*In case we have do any modification affecting the displaying of the object, we redraw them*/
    glutPostRedisplay();
}


void keyboard2(int key, int x, int y){
    GLdouble * matrix = malloc(sizeof(GLdouble)*16);
    matrix[0] = 1;
    matrix[1] = 0;
    matrix[2] = 0;
    matrix[3] = 0;
    matrix[4] = 0;
    matrix[5] = 1;
    matrix[6] = 0;
    matrix[7] = 0;
    matrix[8] = 0;
    matrix[9] = 0;
    matrix[10] = 1;
    matrix[11] = 0;
    matrix[12] = 0;
    matrix[13] = 0;
    matrix[14] = 0;
    matrix[15] = 1;

    
	if(erreferentzia == GLOBALA){
		switch(key){
			case GLUT_KEY_UP:
				if (egoera == TRASLAZIOA){
					matrix[0] = 1;
					matrix[1] = 0;
					matrix[2] = 0;
					matrix[3] = 0;
					matrix[4] = 0;
					matrix[5] = 1;
					matrix[6] = 0;
					matrix[7] = 0;
					matrix[8] = 0;
					matrix[9] = 0;
					matrix[10] = 1;
					matrix[11] = 0;
					matrix[12] = 0;
					matrix[13] = 0.25;
					matrix[14] = 0;
					matrix[15] = 1;
					_selected_object->matrix = matrix_multiplication(matrix, _selected_object->matrix);
				} else if(egoera == BIRAKETA){
					matrix[0] = 1;
					matrix[1] = 0;
					matrix[2] = 0;
					matrix[3] = 0;
					matrix[4] = 0;
					matrix[5] = cos(M_PI/6);
					matrix[6] = sin(M_PI/6);
					matrix[7] = 0;
					matrix[8] = 0;
					matrix[9] = -sin(M_PI/6);
					matrix[10] = cos(M_PI/6);
					matrix[11] = 0;
					matrix[12] = 0;
					matrix[13] = 0;
					matrix[14] = 0;
					matrix[15] = 1;
					_selected_object->matrix = matrix_multiplication(matrix, _selected_object->matrix);
				} else if(egoera == TAMAINA){
					matrix[0] = 1;
					matrix[1] = 0;
					matrix[2] = 0;
					matrix[3] = 0;
					matrix[4] = 0;
					matrix[5] = 1.25;
					matrix[6] = 0;
					matrix[7] = 0;
					matrix[8] = 0;
					matrix[9] = 0;
					matrix[10] = 1;
					matrix[11] = 0;
					matrix[12] = 0;
					matrix[13] = 0;
					matrix[14] = 0;
					matrix[15] = 1;
					_selected_object->matrix = matrix_multiplication(matrix, _selected_object->matrix);
				} 
				break;
			case GLUT_KEY_DOWN:
				if (egoera == TRASLAZIOA){
					matrix[0] = 1;
					matrix[1] = 0;
					matrix[2] = 0;
					matrix[3] = 0;
					matrix[4] = 0;
					matrix[5] = 1;
					matrix[6] = 0;
					matrix[7] = 0;
					matrix[8] = 0;
					matrix[9] = 0;
					matrix[10] = 1;
					matrix[11] = 0;
					matrix[12] = 0;
					matrix[13] = -0.25;
					matrix[14] = 0;
					matrix[15] = 1;
					_selected_object->matrix = matrix_multiplication(matrix, _selected_object->matrix);            
				} else if(egoera == BIRAKETA){
					matrix[0] = 1;
					matrix[1] = 0;
					matrix[2] = 0;
					matrix[3] = 0;
					matrix[4] = 0;
					matrix[5] = cos(M_PI/6);
					matrix[6] = -sin(M_PI/6);
					matrix[7] = 0;
					matrix[8] = 0;
					matrix[9] = sin(M_PI/6);
					matrix[10] = cos(M_PI/6);
					matrix[11] = 0;
					matrix[12] = 0;
					matrix[13] = 0;
					matrix[14] = 0;
					matrix[15] = 1;
					_selected_object->matrix = matrix_multiplication(matrix, _selected_object->matrix);
				} else if(egoera == TAMAINA){
					matrix[0] = 1;
					matrix[1] = 0;
					matrix[2] = 0;
					matrix[3] = 0;
					matrix[4] = 0;
					matrix[5] = 0.8;
					matrix[6] = 0;
					matrix[7] = 0;
					matrix[8] = 0;
					matrix[9] = 0;
					matrix[10] = 1;
					matrix[11] = 0;
					matrix[12] = 0;
					matrix[13] = 0;
					matrix[14] = 0;
					matrix[15] = 1;
					_selected_object->matrix = matrix_multiplication(matrix, _selected_object->matrix);
				} 
				break;
			case GLUT_KEY_RIGHT:
				if (egoera == TRASLAZIOA){
					matrix[0] = 1;
					matrix[1] = 0;
					matrix[2] = 0;
					matrix[3] = 0;
					matrix[4] = 0;
					matrix[5] = 1;
					matrix[6] = 0;
					matrix[7] = 0;
					matrix[8] = 0;
					matrix[9] = 0;
					matrix[10] = 1;
					matrix[11] = 0;
					matrix[12] = 0.25;
					matrix[13] = 0;
					matrix[14] = 0;
					matrix[15] = 1;
					_selected_object->matrix = matrix_multiplication(matrix, _selected_object->matrix);  
				} else if(egoera == BIRAKETA){
					matrix[0] = cos(M_PI/6);
					matrix[1] = 0;
					matrix[2] = -sin(M_PI/6);
					matrix[3] = 0;
					matrix[4] = 0;
					matrix[5] = 1;
					matrix[6] = 0;
					matrix[7] = 0;
					matrix[8] = sin(M_PI/6);
					matrix[9] = 0;
					matrix[10] = cos(M_PI/6);
					matrix[11] = 0;
					matrix[12] = 0;
					matrix[13] = 0;
					matrix[14] = 0;
					matrix[15] = 1;
					_selected_object->matrix = matrix_multiplication(matrix, _selected_object->matrix);
				} else if(egoera == TAMAINA){
					matrix[0] = 1.25;
					matrix[1] = 0;
					matrix[2] = 0;
					matrix[3] = 0;
					matrix[4] = 0;
					matrix[5] = 1;
					matrix[6] = 0;
					matrix[7] = 0;
					matrix[8] = 0;
					matrix[9] = 0;
					matrix[10] = 1;
					matrix[11] = 0;
					matrix[12] = 0;
					matrix[13] = 0;
					matrix[14] = 0;
					matrix[15] = 1;
					_selected_object->matrix = matrix_multiplication(matrix, _selected_object->matrix);
				} 
				break;
			case GLUT_KEY_LEFT:
				if (egoera == TRASLAZIOA){
					matrix[0] = 1;
					matrix[1] = 0;
					matrix[2] = 0;
					matrix[3] = 0;
					matrix[4] = 0;
					matrix[5] = 1;
					matrix[6] = 0;
					matrix[7] = 0;
					matrix[8] = 0;
					matrix[9] = 0;
					matrix[10] = 1;
					matrix[11] = 0;
					matrix[12] = -0.25;
					matrix[13] = 0;
					matrix[14] = 0;
					matrix[15] = 1;
					_selected_object->matrix = matrix_multiplication(matrix, _selected_object->matrix);  
				} else if(egoera == BIRAKETA){
					matrix[0] = cos(M_PI/6);
					matrix[1] = 0;
					matrix[2] = sin(M_PI/6);
					matrix[3] = 0;
					matrix[4] = 0;
					matrix[5] = 1;
					matrix[6] = 0;
					matrix[7] = 0;
					matrix[8] = -sin(M_PI/6);
					matrix[9] = 0;
					matrix[10] = cos(M_PI/6);
					matrix[11] = 0;
					matrix[12] = 0;
					matrix[13] = 0;
					matrix[14] = 0;
					matrix[15] = 1;
					_selected_object->matrix = matrix_multiplication(_selected_object->matrix, matrix);
				} else if(egoera == TAMAINA){
					matrix[0] = 0.8;
					matrix[1] = 0;
					matrix[2] = 0;
					matrix[3] = 0;
					matrix[4] = 0;
					matrix[5] = 1;
					matrix[6] = 0;
					matrix[7] = 0;
					matrix[8] = 0;
					matrix[9] = 0;
					matrix[10] = 1;
					matrix[11] = 0;
					matrix[12] = 0;
					matrix[13] = 0;
					matrix[14] = 0;
					matrix[15] = 1;
					_selected_object->matrix = matrix_multiplication(matrix, _selected_object->matrix);	
				} 
				break;
			case GLUT_KEY_PAGE_UP:
				if (egoera == TRASLAZIOA){
					matrix[0] = 1;
					matrix[1] = 0;
					matrix[2] = 0;
					matrix[3] = 0;
					matrix[4] = 0;
					matrix[5] = 1;
					matrix[6] = 0;
					matrix[7] = 0;
					matrix[8] = 0;
					matrix[9] = 0;
					matrix[10] = 1;
					matrix[11] = 0;
					matrix[12] = 0;
					matrix[13] = 0;
					matrix[14] = 0.25;
					matrix[15] = 1;
					_selected_object->matrix = matrix_multiplication(matrix, _selected_object->matrix);  
				} else if(egoera == BIRAKETA){
					matrix[0] = cos(M_PI/6);
					matrix[1] = sin(M_PI/6);
					matrix[2] = 0;
					matrix[3] = 0;
					matrix[4] = -sin(M_PI/6);
					matrix[5] = cos(M_PI/6);
					matrix[6] = 0;
					matrix[7] = 0;
					matrix[8] = 0;
					matrix[9] = 0;
					matrix[10] = 1;
					matrix[11] = 0;
					matrix[12] = 0;
					matrix[13] = 0;
					matrix[14] = 0;
					matrix[15] = 1;
					_selected_object->matrix = matrix_multiplication(matrix, _selected_object->matrix);
				} else if(egoera == TAMAINA){
					matrix[0] = 1;
					matrix[1] = 0;
					matrix[2] = 0;
					matrix[3] = 0;
					matrix[4] = 0;
					matrix[5] = 1;
					matrix[6] = 0;
					matrix[7] = 0;
					matrix[8] = 0;
					matrix[9] = 0;
					matrix[10] = 1.25;
					matrix[11] = 0;
					matrix[12] = 0;
					matrix[13] = 0;
					matrix[14] = 0;
					matrix[15] = 1;
					_selected_object->matrix = matrix_multiplication(matrix, _selected_object->matrix);
				} 
				break;
			case GLUT_KEY_PAGE_DOWN:
				if (egoera == TRASLAZIOA){
					matrix[0] = 1;
					matrix[1] = 0;
					matrix[2] = 0;
					matrix[3] = 0;
					matrix[4] = 0;
					matrix[5] = 1;
					matrix[6] = 0;
					matrix[7] = 0;
					matrix[8] = 0;
					matrix[9] = 0;
					matrix[10] = 1;
					matrix[11] = 0;
					matrix[12] = 0;
					matrix[13] = 0;
					matrix[14] = -0.25;
					matrix[15] = 1;
					_selected_object->matrix = matrix_multiplication(matrix, _selected_object->matrix);  
				} else if(egoera == BIRAKETA){
					matrix[0] = cos(M_PI/6);
					matrix[1] = -sin(M_PI/6);
					matrix[2] = 0;
					matrix[3] = 0;
					matrix[4] = sin(M_PI/6);
					matrix[5] = cos(M_PI/6);
					matrix[6] = 0;
					matrix[7] = 0;
					matrix[8] = 0;
					matrix[9] = 0;
					matrix[10] = 1;
					matrix[11] = 0;
					matrix[12] = 0;
					matrix[13] = 0;
					matrix[14] = 0;
					matrix[15] = 1;
					_selected_object->matrix = matrix_multiplication(matrix, _selected_object->matrix);
				} else if(egoera == TAMAINA){
					matrix[0] = 1;
					matrix[1] = 0;
					matrix[2] = 0;
					matrix[3] = 0;
					matrix[4] = 0;
					matrix[5] = 1;
					matrix[6] = 0;
					matrix[7] = 0;
					matrix[8] = 0;
					matrix[9] = 0;
					matrix[10] = 0.8;
					matrix[11] = 0;
					matrix[12] = 0;
					matrix[13] = 0;
					matrix[14] = 0;
					matrix[15] = 1;
					_selected_object->matrix = matrix_multiplication(matrix, _selected_object->matrix);
				}
		}
    } else if(erreferentzia == LOKALA){
		switch(key){
			case GLUT_KEY_UP:
				if (egoera == TRASLAZIOA){
					matrix[0] = 1;
					matrix[1] = 0;
					matrix[2] = 0;
					matrix[3] = 0;
					matrix[4] = 0;
					matrix[5] = 1;
					matrix[6] = 0;
					matrix[7] = 0;
					matrix[8] = 0;
					matrix[9] = 0;
					matrix[10] = 1;
					matrix[11] = 0;
					matrix[12] = 0;
					matrix[13] = 0.25;
					matrix[14] = 0;
					matrix[15] = 1;
					_selected_object->matrix = matrix_multiplication(_selected_object->matrix, matrix);
				} else if(egoera == BIRAKETA){
					matrix[0] = 1;
					matrix[1] = 0;
					matrix[2] = 0;
					matrix[3] = 0;
					matrix[4] = 0;
					matrix[5] = cos(M_PI/6);
					matrix[6] = sin(M_PI/6);
					matrix[7] = 0;
					matrix[8] = 0;
					matrix[9] = -sin(M_PI/6);
					matrix[10] = cos(M_PI/6);
					matrix[11] = 0;
					matrix[12] = 0;
					matrix[13] = 0;
					matrix[14] = 0;
					matrix[15] = 1;
					_selected_object->matrix = matrix_multiplication(_selected_object->matrix, matrix);
				} else if(egoera == TAMAINA){
					matrix[0] = 1;
					matrix[1] = 0;
					matrix[2] = 0;
					matrix[3] = 0;
					matrix[4] = 0;
					matrix[5] = 1.25;
					matrix[6] = 0;
					matrix[7] = 0;
					matrix[8] = 0;
					matrix[9] = 0;
					matrix[10] = 1;
					matrix[11] = 0;
					matrix[12] = 0;
					matrix[13] = 0;
					matrix[14] = 0;
					matrix[15] = 1;
					_selected_object->matrix = matrix_multiplication(_selected_object->matrix, matrix);
				} 
				break;
			case GLUT_KEY_DOWN:
				if (egoera == TRASLAZIOA){
					matrix[0] = 1;
					matrix[1] = 0;
					matrix[2] = 0;
					matrix[3] = 0;
					matrix[4] = 0;
					matrix[5] = 1;
					matrix[6] = 0;
					matrix[7] = 0;
					matrix[8] = 0;
					matrix[9] = 0;
					matrix[10] = 1;
					matrix[11] = 0;
					matrix[12] = 0;
					matrix[13] = -0.25;
					matrix[14] = 0;
					matrix[15] = 1;
					_selected_object->matrix = matrix_multiplication(_selected_object->matrix, matrix);            
				} else if(egoera == BIRAKETA){
					matrix[0] = 1;
					matrix[1] = 0;
					matrix[2] = 0;
					matrix[3] = 0;
					matrix[4] = 0;
					matrix[5] = cos(M_PI/6);
					matrix[6] = -sin(M_PI/6);
					matrix[7] = 0;
					matrix[8] = 0;
					matrix[9] = sin(M_PI/6);
					matrix[10] = cos(M_PI/6);
					matrix[11] = 0;
					matrix[12] = 0;
					matrix[13] = 0;
					matrix[14] = 0;
					matrix[15] = 1;
					_selected_object->matrix = matrix_multiplication(_selected_object->matrix, matrix);
				} else if(egoera == TAMAINA){
					matrix[0] = 1;
					matrix[1] = 0;
					matrix[2] = 0;
					matrix[3] = 0;
					matrix[4] = 0;
					matrix[5] = 0.8;
					matrix[6] = 0;
					matrix[7] = 0;
					matrix[8] = 0;
					matrix[9] = 0;
					matrix[10] = 1;
					matrix[11] = 0;
					matrix[12] = 0;
					matrix[13] = 0;
					matrix[14] = 0;
					matrix[15] = 1;
					_selected_object->matrix = matrix_multiplication(_selected_object->matrix, matrix);
				} 
				break;
			case GLUT_KEY_RIGHT:
				if (egoera == TRASLAZIOA){
					matrix[0] = 1;
					matrix[1] = 0;
					matrix[2] = 0;
					matrix[3] = 0;
					matrix[4] = 0;
					matrix[5] = 1;
					matrix[6] = 0;
					matrix[7] = 0;
					matrix[8] = 0;
					matrix[9] = 0;
					matrix[10] = 1;
					matrix[11] = 0;
					matrix[12] = 0.25;
					matrix[13] = 0;
					matrix[14] = 0;
					matrix[15] = 1;
					_selected_object->matrix = matrix_multiplication(_selected_object->matrix, matrix);  
				} else if(egoera == BIRAKETA){
					matrix[0] = cos(M_PI/6);
					matrix[1] = 0;
					matrix[2] = -sin(M_PI/6);
					matrix[3] = 0;
					matrix[4] = 0;
					matrix[5] = 1;
					matrix[6] = 0;
					matrix[7] = 0;
					matrix[8] = sin(M_PI/6);
					matrix[9] = 0;
					matrix[10] = cos(M_PI/6);
					matrix[11] = 0;
					matrix[12] = 0;
					matrix[13] = 0;
					matrix[14] = 0;
					matrix[15] = 1;
					_selected_object->matrix = matrix_multiplication(_selected_object->matrix, matrix);
				} else if(egoera == TAMAINA){
					matrix[0] = 1.25;
					matrix[1] = 0;
					matrix[2] = 0;
					matrix[3] = 0;
					matrix[4] = 0;
					matrix[5] = 1;
					matrix[6] = 0;
					matrix[7] = 0;
					matrix[8] = 0;
					matrix[9] = 0;
					matrix[10] = 1;
					matrix[11] = 0;
					matrix[12] = 0;
					matrix[13] = 0;
					matrix[14] = 0;
					matrix[15] = 1;
					_selected_object->matrix = matrix_multiplication(_selected_object->matrix, matrix);
				} 
				break;
			case GLUT_KEY_LEFT:
				if (egoera == TRASLAZIOA){
					matrix[0] = 1;
					matrix[1] = 0;
					matrix[2] = 0;
					matrix[3] = 0;
					matrix[4] = 0;
					matrix[5] = 1;
					matrix[6] = 0;
					matrix[7] = 0;
					matrix[8] = 0;
					matrix[9] = 0;
					matrix[10] = 1;
					matrix[11] = 0;
					matrix[12] = -0.25;
					matrix[13] = 0;
					matrix[14] = 0;
					matrix[15] = 1;
					_selected_object->matrix = matrix_multiplication(_selected_object->matrix, matrix);  
				} else if(egoera == BIRAKETA){
					matrix[0] = cos(M_PI/6);
					matrix[1] = 0;
					matrix[2] = sin(M_PI/6);
					matrix[3] = 0;
					matrix[4] = 0;
					matrix[5] = 1;
					matrix[6] = 0;
					matrix[7] = 0;
					matrix[8] = -sin(M_PI/6);
					matrix[9] = 0;
					matrix[10] = cos(M_PI/6);
					matrix[11] = 0;
					matrix[12] = 0;
					matrix[13] = 0;
					matrix[14] = 0;
					matrix[15] = 1;
					_selected_object->matrix = matrix_multiplication(_selected_object->matrix, matrix);
				} else if(egoera == TAMAINA){
					matrix[0] = 0.8;
					matrix[1] = 0;
					matrix[2] = 0;
					matrix[3] = 0;
					matrix[4] = 0;
					matrix[5] = 1;
					matrix[6] = 0;
					matrix[7] = 0;
					matrix[8] = 0;
					matrix[9] = 0;
					matrix[10] = 1;
					matrix[11] = 0;
					matrix[12] = 0;
					matrix[13] = 0;
					matrix[14] = 0;
					matrix[15] = 1;
					_selected_object->matrix = matrix_multiplication(_selected_object->matrix, matrix);	
				} 
				break;
			case GLUT_KEY_PAGE_UP:
				if (egoera == TRASLAZIOA){
					matrix[0] = 1;
					matrix[1] = 0;
					matrix[2] = 0;
					matrix[3] = 0;
					matrix[4] = 0;
					matrix[5] = 1;
					matrix[6] = 0;
					matrix[7] = 0;
					matrix[8] = 0;
					matrix[9] = 0;
					matrix[10] = 1;
					matrix[11] = 0;
					matrix[12] = 0;
					matrix[13] = 0;
					matrix[14] = 0.25;
					matrix[15] = 1;
					_selected_object->matrix = matrix_multiplication(_selected_object->matrix, matrix);  
				} else if(egoera == BIRAKETA){
					matrix[0] = cos(M_PI/6);
					matrix[1] = sin(M_PI/6);
					matrix[2] = 0;
					matrix[3] = 0;
					matrix[4] = -sin(M_PI/6);
					matrix[5] = cos(M_PI/6);
					matrix[6] = 0;
					matrix[7] = 0;
					matrix[8] = 0;
					matrix[9] = 0;
					matrix[10] = 1;
					matrix[11] = 0;
					matrix[12] = 0;
					matrix[13] = 0;
					matrix[14] = 0;
					matrix[15] = 1;
					_selected_object->matrix = matrix_multiplication(_selected_object->matrix, matrix);
				} else if(egoera == TAMAINA){
					matrix[0] = 1;
					matrix[1] = 0;
					matrix[2] = 0;
					matrix[3] = 0;
					matrix[4] = 0;
					matrix[5] = 1;
					matrix[6] = 0;
					matrix[7] = 0;
					matrix[8] = 0;
					matrix[9] = 0;
					matrix[10] = 1.25;
					matrix[11] = 0;
					matrix[12] = 0;
					matrix[13] = 0;
					matrix[14] = 0;
					matrix[15] = 1;
					_selected_object->matrix = matrix_multiplication(_selected_object->matrix, matrix);
				} 
				break;
			case GLUT_KEY_PAGE_DOWN:
				if (egoera == TRASLAZIOA){
					matrix[0] = 1;
					matrix[1] = 0;
					matrix[2] = 0;
					matrix[3] = 0;
					matrix[4] = 0;
					matrix[5] = 1;
					matrix[6] = 0;
					matrix[7] = 0;
					matrix[8] = 0;
					matrix[9] = 0;
					matrix[10] = 1;
					matrix[11] = 0;
					matrix[12] = 0;
					matrix[13] = 0;
					matrix[14] = -0.25;
					matrix[15] = 1;
					_selected_object->matrix = matrix_multiplication(_selected_object->matrix, matrix);  
				} else if(egoera == BIRAKETA){
					matrix[0] = cos(M_PI/6);
					matrix[1] = -sin(M_PI/6);
					matrix[2] = 0;
					matrix[3] = 0;
					matrix[4] = sin(M_PI/6);
					matrix[5] = cos(M_PI/6);
					matrix[6] = 0;
					matrix[7] = 0;
					matrix[8] = 0;
					matrix[9] = 0;
					matrix[10] = 1;
					matrix[11] = 0;
					matrix[12] = 0;
					matrix[13] = 0;
					matrix[14] = 0;
					matrix[15] = 1;
					_selected_object->matrix = matrix_multiplication(_selected_object->matrix, matrix);
				} else if(egoera == TAMAINA){
					matrix[0] = 1;
					matrix[1] = 0;
					matrix[2] = 0;
					matrix[3] = 0;
					matrix[4] = 0;
					matrix[5] = 1;
					matrix[6] = 0;
					matrix[7] = 0;
					matrix[8] = 0;
					matrix[9] = 0;
					matrix[10] = 0.8;
					matrix[11] = 0;
					matrix[12] = 0;
					matrix[13] = 0;
					matrix[14] = 0;
					matrix[15] = 1;
					_selected_object->matrix = matrix_multiplication(_selected_object->matrix, matrix);
				}
		}
     }else{

	 }
	/*In case we have do any modification affecting the displaying of the object, we redraw them*/
    glutPostRedisplay();
}

GLdouble * matrix_multiplication(GLdouble * a, GLdouble * b){
	GLdouble * c = malloc(sizeof(GLdouble)*16);
	int x = 0;
	for(int i=0; i<4; i++){
		for(int j=0; j<4; j++){
			c[x+j] = a[j]*b[x]+a[j+4]*b[x+1]+a[j+8]*b[x+2]+a[j+12]*b[x+3];
		}
		x = x+4;
	}
	return c;
}
