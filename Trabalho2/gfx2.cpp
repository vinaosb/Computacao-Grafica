#include <gtk/gtk.h>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <math.h>

#define moveSpace 20
#define PI 3.14159265

static cairo_surface_t *surface = NULL;
GtkWidget *window;
GtkWidget *drawing_area;
GtkWidget *textview;

GtkWidget *gx1,*gx2,*gx3,*gy1,*gy2,*gy3,*name,*p_entry_x,*p_entry_y;
GtkTextBuffer *buffer;
GtkTextMark *mark;
GtkTextIter iter;

typedef struct {
    //tipo 0 = ponto, 1 = linha, 2 = triangulo
    double x1,x2,x3,y1,y2,y3;
    std::string name;
    int tipo = 0;
} object;

static int xvp = 400, yvp = 400, xwmin = 0, xwmax = 400, ywmin = 0, ywmax = 400, pointWidth = 4;

static double zoom = 1, anyPoint_x = 0, anyPoint_y = 0;

std::vector<object> listObjects;

static void print_list(void)
{
    for (std::vector<object>::iterator it=listObjects.begin(); it != listObjects.end(); ++it) {
        if (it->x1 == it->x2 && it->y1 == it->y2) {
            std::cout << "Nome: " << it->name << " X1 = " << it->x1 << " Y1 = " << it->y1 << std::endl;
        } else {
            std::cout << 
            "Nome: " << it->name <<
            " Tipo: " << it->tipo <<
            " X1 = " << it->x1 << 
            " Y1 = " << it->y1 << 
            " X2 = " << it->x2 << 
            " Y2 = " << it->y2 << 
            std::endl;
        }
    }
    std::cout << std::endl << std::endl << std::endl;
}

void rotation (object *o, double a) {
    std::cout<<"P1("<< o->x1<<","<<o->y1<<"); P2("<<o->x2<<","<<o->y2<<")\nRotate "<<a<<"º:\n";
    a = a * PI/180.0;
    double aux_x1, aux_x2, aux_x3, aux_y1, aux_y2, aux_y3;
    aux_x1 = o->x1;
    aux_x2 = o->x2;
    aux_x3 = o->x3;
    aux_y1 = o->y1;
    aux_y2 = o->y2;
    aux_y3 = o->y3;
    
    o->x1 = aux_x1 * cos ( a ) + aux_y1 * sin (a);
    o->y1 = aux_y1 * cos ( a ) - aux_x1 * sin (a);
    o->x2 = aux_x2 * cos ( a ) + aux_y2 * sin (a);
    o->y2 = aux_y2 * cos ( a ) - aux_x2 * sin (a);
    o->x3 = aux_x3 * cos ( a ) + aux_y3 * sin (a);
    o->y3 = aux_y3 * cos ( a ) - aux_x3 * sin (a);
    std::cout<<"P1("<< o->x1<<","<<o->y1<<"); P2("<<o->x2<<","<<o->y2<<")\n";
}

void translation (object *o, double dx, double dy) {
    o->x1 = o->x1 + dx;
    o->x2 = o->x2 + dx;
    o->x3 = o->x3 + dx;
    o->y1 = o->y1 + dy;
    o->y2 = o->y2 + dy;
    o->y3 = o->y3 + dy;
}
static object* findCenter(object *o){
    object *oc;
    oc->x1 = (o->x1+o->x2+o->x3)/3;
    oc->y1 = (o->y1+o->y2+o->x3)/3;
   
    return oc;
}
void scaling (object *o, double e) {
    if (o->tipo != 0){
        object *oc = findCenter(o);
        translation(o, -oc->x1, -oc->y1);
        o->x1 = o->x1 * e;
        o->x2 = o->x2 * e;
        o->x3 = o->x3 * e;
        o->y1 = o->y1 * e;
        o->y2 = o->y2 * e;
        o->y3 = o->y3 * e;
        translation(o, oc->x1, oc->y1);
    }
}

static void clear_surface (void)
{	 	  	 	    	   	      	   	 	    	    	 	
  cairo_t *cr;
  cr = cairo_create (surface);
  cairo_set_source_rgb (cr, 1, 1, 1);
  cairo_paint (cr);
  cairo_destroy (cr);
}

static void draw_point(int x, int y)
{
  cairo_t *cr;
  cr = cairo_create (surface);  
  cairo_set_line_width (cr, pointWidth);
  cairo_set_line_cap  (cr, CAIRO_LINE_CAP_ROUND); /* Round dot*/
  
   double xf = ((x - xwmin)*(xvp)/((xwmax-xwmin)*zoom));
   double yf = (1 - ((y - ywmin)/((ywmax-ywmin)*zoom)))*(yvp);
  
  cairo_move_to(cr, xf, yf);
  cairo_line_to(cr, xf, yf);
  cairo_stroke(cr);
  gtk_widget_queue_draw (window);
}

static void draw_line(double x1, double x2, double y1, double y2) 
{
   cairo_t *cr;
   cr = cairo_create (surface);
   double xf1 = ((x1 - xwmin)*(xvp)/((xwmax-xwmin)*zoom));
   double xf2 = ((x2 - xwmin)*(xvp)/((xwmax-xwmin)*zoom));
   double yf1 = (1 - ((y1 - ywmin)/((ywmax-ywmin)*zoom)))*(yvp);
   double yf2 = (1 - ((y2 - ywmin)/((ywmax-ywmin)*zoom)))*(yvp);
   cairo_move_to(cr, xf1, yf1);
   cairo_line_to(cr, xf2, yf2);
   cairo_stroke(cr);
   gtk_widget_queue_draw (window);
}

static void draw_tritangle(double x1, double x2, double x3, double y1, double y2, double y3) 
{
   cairo_t *cr;
   cr = cairo_create (surface);

   double xf1 = ((x1 - xwmin)*(xvp)/((xwmax-xwmin)*zoom));
   double xf2 = ((x2 - xwmin)*(xvp)/((xwmax-xwmin)*zoom));
   double xf3 = ((x3 - xwmin)*(xvp)/((xwmax-xwmin)*zoom));
   double yf1 = (1 - ((y1 - ywmin)/((ywmax-ywmin)*zoom)))*(yvp);
   double yf2 = (1 - ((y2 - ywmin)/((ywmax-ywmin)*zoom)))*(yvp);
   double yf3 = (1 - ((y3 - ywmin)/((ywmax-ywmin)*zoom)))*(yvp);
   
   cairo_move_to(cr, xf1, yf1);
   cairo_line_to(cr, xf3, yf3);
   cairo_line_to(cr, xf2, yf2);
   cairo_line_to(cr, xf1, yf1);
   
   cairo_stroke(cr);
   gtk_widget_queue_draw (window);
}

static void redraw (void)
{  	 	    	    	 	
    clear_surface();
    
    for (std::vector<object>::iterator it=listObjects.begin(); it != listObjects.end(); ++it) {
        switch (it->tipo) {
            case 1 :
                draw_line(it->x1,it->x2,it->y1,it->y2);
                break;
            case 2 :
                draw_tritangle(it->x1,it->x2,it->x3,it->y1,it->y2,it->y3);
                break;
            default :
                draw_point(it->x1,it->y1);
        }
    }
}

static void move (int x, int y)
{
    for (std::vector<object>::iterator it=listObjects.begin(); it != listObjects.end(); ++it) {
        it->x1 = it->x1 + x;
        it->x2 = it->x2 + x;
        it->x3 = it->x3 + x;
        it->y1 = it->y1 + y;
        it->y2 = it->y2 + y;
        it->y3 = it->y3 + y;
    }
    redraw();
}
static void moveUp(){
    //move(0,-moveSpace);
    ywmin = ywmin + moveSpace;
    ywmax = ywmax + moveSpace;
    redraw();
}
static void moveLeft (){
    //move(moveSpace,0);
    xwmin = xwmin - moveSpace;
    xwmax = xwmax - moveSpace;
    redraw();
}

static void moveRight (){
    //move(-moveSpace,0);
    xwmin = xwmin + moveSpace;
    xwmax = xwmax + moveSpace;
    redraw();
}

static void moveDown (){
    //move(0,moveSpace);
    ywmin = ywmin - moveSpace;
    ywmax = ywmax - moveSpace;
    redraw();
}

static void zoomWindow (bool z)
{
    if (!z)  {
        //zoom = zoom + 0.1;
        //pointWidth--;
        xwmax = xwmax + moveSpace;
        xwmin = xwmin - moveSpace;
        ywmax = ywmax + moveSpace;
        ywmin = ywmin - moveSpace;
    }
    else {
        //zoom = zoom - 0.1;
        //pointWidth++;
        xwmax = xwmax - moveSpace;
        xwmin = xwmin + moveSpace;
        ywmax = ywmax - moveSpace;
        ywmin = ywmin + moveSpace;
    }
    redraw();
}

static void zoomIn () {
    zoomWindow (TRUE);
}

static void zoomOut() {
    zoomWindow (FALSE);
}

static gboolean configure_event_cb (GtkWidget *widget, GdkEventConfigure *event, gpointer data){
  if (surface)
    cairo_surface_destroy (surface);

  surface = gdk_window_create_similar_surface (gtk_widget_get_window (widget),
                                       CAIRO_CONTENT_COLOR,
                                       gtk_widget_get_allocated_width (widget),
                                       gtk_widget_get_allocated_height (widget));
  clear_surface ();
  return TRUE;
}
static gboolean draw_cb (GtkWidget *widget, cairo_t   *cr,  gpointer   data){
  cairo_set_source_surface (cr, surface, 0, 0);
  cairo_paint (cr);

  return FALSE;
}

static void insert_text_buffer(object obj){
    std::string tipo;
    switch(obj.tipo){
        case 1:
            tipo = "Linha";
            break;
        case 2:
            tipo = "Triangulo";
            break;
        default:
            tipo = "Ponto";
    }
  std::string text = obj.name + " - " + tipo;
  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview));
  mark = gtk_text_buffer_get_insert (buffer);
  gtk_text_buffer_get_iter_at_mark (buffer, &iter, mark);
  if (gtk_text_buffer_get_char_count(buffer))
    gtk_text_buffer_insert (buffer, &iter, "\n", 1);
  gtk_text_buffer_insert (buffer, &iter, text.c_str(), -1);
}

static void btn_point_clicked_cb(){
  std::string tname(gtk_entry_get_text(GTK_ENTRY(name)));
  std::string tx1(gtk_entry_get_text(GTK_ENTRY(gx1)));
  std::string ty1(gtk_entry_get_text(GTK_ENTRY(gy1)));
  
  object obj;
  obj.x1 = stod(tx1);
  obj.x2 = stod(tx1);
  obj.x3 = stod(tx1);
  obj.y1 = stod(ty1);
  obj.y2 = stod(ty1);
  obj.y3 = stod(ty1);
  
  obj.name = tname;
  obj.tipo = 0;
  listObjects.push_back(obj);

  //insere objeto no TextView
  insert_text_buffer(obj);
  redraw();
}	 	  	 	    	   	      	   	 	    	    	 	
static void btn_line_clicked_cb(){
  std::string tname(gtk_entry_get_text(GTK_ENTRY(name)));
  std::string tx1(gtk_entry_get_text(GTK_ENTRY(gx1)));
  std::string tx2(gtk_entry_get_text(GTK_ENTRY(gx2)));
  std::string ty1(gtk_entry_get_text(GTK_ENTRY(gy1)));
  std::string ty2(gtk_entry_get_text(GTK_ENTRY(gy2)));
  
  object obj;
  obj.x1 = stod(tx1);
  obj.x2 = stod(tx2);
  obj.x3 = (stod(tx1)+stod(tx2))/2;
  obj.y1 = stod(ty1);
  obj.y2 = stod(ty2);
  obj.y3 = (stod(ty1)+stod(ty2))/2;
  
  
  obj.name = tname;
  obj.tipo = 1;
  listObjects.push_back(obj);
  
  //insere objeto no TextView
  insert_text_buffer(obj);
  
  redraw();
  //draw_line(obj.x1,obj.x2,obj.y1,obj.y2);
 } 
static void btn_rectangle_clicked_cb(){
   std::string tname(gtk_entry_get_text(GTK_ENTRY(name)));
   std::string tx1(gtk_entry_get_text(GTK_ENTRY(gx1)));
   std::string tx2(gtk_entry_get_text(GTK_ENTRY(gx2)));
   std::string tx3(gtk_entry_get_text(GTK_ENTRY(gx3)));
   std::string ty1(gtk_entry_get_text(GTK_ENTRY(gy1)));
   std::string ty2(gtk_entry_get_text(GTK_ENTRY(gy2)));
   std::string ty3(gtk_entry_get_text(GTK_ENTRY(gy3)));
   
   object obj;
   obj.x1 = stod(tx1);
   obj.x2 = stod(tx2);
   obj.x3 = stod(tx3);
   obj.y1 = stod(ty1);
   obj.y2 = stod(ty2);
   obj.y3 = stod(ty3);
   
   obj.name = tname;
   obj.tipo = 2;
   listObjects.push_back(obj);
  
   //insere objeto no TextView
   insert_text_buffer(obj);
   
   redraw();
  }
  
  static object* getObject(){
    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview));
    mark = gtk_text_buffer_get_selection_bound (buffer);
    gtk_text_buffer_get_iter_at_mark (buffer, &iter, mark);
    int linha_cursor = gtk_text_iter_get_line (&iter);
    object *o = &listObjects.at(linha_cursor);
    std::cout<<"objeto: " << o->name << ".\n";
    return o;
      
  }
  static void translateObj(){
    object *o = getObject();
    translation(o,10,10);
    redraw();
  }
  static void scaleObj(){
    object *o = getObject();
    scaling(o,1.1);
    redraw();
  }
//   void exec_rotate(object *o){//GERA ERRO NA SEGUNDA ROTACAO
//     object *oc = findCenter(o);
//     translation(o, -oc->x1, -oc->y1);
//     rotation(o,30);
//     translation(o, oc->x1, oc->y1);

//  }
 static void rotateByObjCenter(){
    object *o = getObject();
    // object *oc = findCenter(o);//AQUI GERA ERRO
    // translation(o, -oc->x1, -oc->y1);
    // rotation(o,30);
    // translation(o, oc->x1, oc->y1);
    //exec_rotate(o);
    double xc = (o->x1 + o->x2 + o->x3)/3;
    double yc = (o->y1 + o->y2 + o->y3)/3;
    translation(o,-xc,-yc);
    rotation(o,30);
    translation(o,xc,yc);
    redraw();
 };
 static void rotateByWorldCenter(){
    object *o = getObject();
    rotation(o,30);
    redraw();
 };
 static void rotateByAnyPoint(){
    object *o = getObject();
    std::string px(gtk_entry_get_text(GTK_ENTRY(p_entry_x)));
    std::string py(gtk_entry_get_text(GTK_ENTRY(p_entry_y)));
    anyPoint_x = stod(px);
    anyPoint_y = stod(py);
    translation(o,-anyPoint_x,-anyPoint_y);
    rotation(o,30);
    translation(o,anyPoint_x,anyPoint_y);
    redraw();
};
 static void btn_clear_clicked_cb(){	 	  	 	    	   	      	   	 	    	    	 	
    // apaga buffer do textView
    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview));
    gtk_text_buffer_set_text(buffer,"",-1);
    
    clear_surface();
    gtk_widget_queue_draw (window);
    while(!listObjects.empty())
        listObjects.pop_back();
  }
void create_new_window(){
    //gtk_button_released(p_widget);
    GtkWidget *p_window;
    GtkWidget *grid;
    GtkWidget *p_label_x,*p_label_main,*p_label_y,*p_button, *space;

    p_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_position(GTK_WINDOW(p_window), GTK_WIN_POS_CENTER);
    gtk_window_set_title(GTK_WINDOW(p_window), "Rotação sobre um Ponto Qualquer");
    gtk_window_set_default_size(GTK_WINDOW(p_window), 200, 150);

    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(p_window), grid);
    
    p_label_main = gtk_label_new("Ponto à rotacionar:");
    gtk_grid_attach(GTK_GRID(grid), p_label_main,0,0,3,2);
    p_label_x = gtk_label_new("X = ");
    gtk_grid_attach(GTK_GRID(grid),p_label_x,1,2,1,2);
    p_entry_x = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), p_entry_x,2,2,1,2);
    p_label_y= gtk_label_new("Y = ");
    gtk_grid_attach(GTK_GRID(grid), p_label_y,1,4,1,2);
    p_entry_y = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), p_entry_y,2,4,1,2);
    space = gtk_label_new("");
    gtk_grid_attach(GTK_GRID(grid), space,2,6,1,1);
    p_button = gtk_button_new_with_label("Rotacionar");
    gtk_grid_attach(GTK_GRID(grid), p_button,2,7,1,1);
    
    gtk_entry_set_text(GTK_ENTRY(p_entry_x),"0");
    gtk_entry_set_text(GTK_ENTRY(p_entry_y),"0");
    
    g_signal_connect(p_button,"clicked",G_CALLBACK(rotateByAnyPoint),p_window);
    g_signal_connect_swapped(p_button,"clicked",G_CALLBACK(gtk_widget_destroy),p_window);

    gtk_widget_show_all(p_window);
}

int main (int argc, char *argv[])
{
    GtkWidget *label;
    GtkWidget *button;
    GtkWidget *grid;
    GtkWidget *frame;
    GtkWidget *scrolled_win,*hbox,*vbox;

    gtk_init(&argc, &argv);

    /* Create the main, top level window */
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    /* Give it the title */
    gtk_window_set_title(GTK_WINDOW(window), "Hello, world!");

    /* Center the window */
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

    /* Set the window's default size */
    gtk_window_set_default_size(GTK_WINDOW(window), 720, 480);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    grid = gtk_grid_new();

    /* Plot the label onto the main window */
    gtk_container_add(GTK_CONTAINER(window), grid);
    
    label = gtk_label_new("Name = ");
    gtk_grid_attach(GTK_GRID(grid), label, 0,3,1,1);
    label = gtk_label_new("x1 = ");
    gtk_grid_attach(GTK_GRID(grid), label, 0,4,1,1);
    label = gtk_label_new("y1 = ");
    gtk_grid_attach(GTK_GRID(grid), label, 0,5,1,1);
    label = gtk_label_new("x2 = ");
    gtk_grid_attach(GTK_GRID(grid), label, 0,6,1,1);
    label = gtk_label_new("y2 = ");
    gtk_grid_attach(GTK_GRID(grid), label, 0,7,1,1);
    label = gtk_label_new("x3 = ");
    gtk_grid_attach(GTK_GRID(grid), label, 0,8,1,1);
    label = gtk_label_new("y3 = ");
    gtk_grid_attach(GTK_GRID(grid), label, 0,9,1,1);

    label = gtk_label_new("Objects");
    gtk_grid_attach(GTK_GRID(grid), label, 0,10,1,1);
    
    //button_box=gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
    //gtk_container_add(GTK_CONTAINER(window), button_box);
    
    button=gtk_button_new_with_label("Close");
    g_signal_connect(button,"clicked",G_CALLBACK(print_list),window);
    g_signal_connect_swapped(button,"clicked",G_CALLBACK(gtk_widget_destroy),window);
    //gtk_container_add(GTK_CONTAINER(button_box), button);
    gtk_grid_attach(GTK_GRID(grid), button, 0,20,1,1);
    
    name=gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(name),"Objeto1");
    gtk_grid_attach(GTK_GRID(grid), name, 1,3,2,1);
    gx1=gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(gx1),"0");
    gtk_grid_attach(GTK_GRID(grid), gx1, 1,4,2,1);
    gy1=gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(gy1),"0");
    gtk_grid_attach(GTK_GRID(grid), gy1, 1,5,2,1);
    gx2=gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(gx2),"0");
    gtk_grid_attach(GTK_GRID(grid), gx2, 1,6,2,1);
    gy2=gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(gy2),"0");
    gtk_grid_attach(GTK_GRID(grid), gy2, 1,7,2,1);
    gx3=gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(gx3),"0");
    gtk_grid_attach(GTK_GRID(grid), gx3, 1,8,2,1);
    gy3=gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(gy3),"0");
    gtk_grid_attach(GTK_GRID(grid), gy3, 1,9,2,1);
    
    button=gtk_button_new_with_label("Point");
    g_signal_connect(button,"clicked",G_CALLBACK(btn_point_clicked_cb),window);
    gtk_grid_attach(GTK_GRID(grid), button, 0,2,1,1);

    button=gtk_button_new_with_label("Line");
    g_signal_connect(button,"clicked",G_CALLBACK(btn_line_clicked_cb),window);
    gtk_grid_attach(GTK_GRID(grid), button, 1,2,1,1);

    button=gtk_button_new_with_label("Triangle");
    g_signal_connect(button,"clicked",G_CALLBACK(btn_rectangle_clicked_cb),window);
    gtk_grid_attach(GTK_GRID(grid), button, 2,2,1,1);

    //Text View para listar os Objetos desenhados
    textview = gtk_text_view_new ();
    scrolled_win = gtk_scrolled_window_new (NULL, NULL);
    gtk_container_add (GTK_CONTAINER (scrolled_win), textview);

    hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 5);
    //gtk_box_pack_start_defaults (GTK_BOX (hbox), insert);

    vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start (GTK_BOX (vbox), scrolled_win, TRUE, TRUE, 0);
    gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);
    gtk_grid_attach(GTK_GRID(grid), vbox, 0,11,3,8);
    
    //Fim do trecho TextView
    
    button=gtk_button_new_with_label("Clear");
    g_signal_connect(button,"clicked",G_CALLBACK(btn_clear_clicked_cb),window);
    gtk_grid_attach(GTK_GRID(grid), button, 0,0,1,1);
    
    gtk_container_set_border_width (GTK_CONTAINER (window), 8);
    frame = gtk_frame_new(NULL);
    gtk_grid_attach(GTK_GRID(grid),frame,4,0,20,20);
    drawing_area = gtk_drawing_area_new ();
    
       //botoes a direita da window
    button=gtk_button_new_with_label("Move Up");
    g_signal_connect(button,"clicked",G_CALLBACK(moveUp),window);
    gtk_grid_attach(GTK_GRID(grid), button, 25,0,2,1);
        //
    button=gtk_button_new_with_label("Move Left");
    g_signal_connect(button,"clicked",G_CALLBACK(moveLeft),window);
    gtk_grid_attach(GTK_GRID(grid), button, 25,1,1,1);
        //
    button=gtk_button_new_with_label("Move Right");
    g_signal_connect(button,"clicked",G_CALLBACK(moveRight),window);
    gtk_grid_attach(GTK_GRID(grid), button, 26,1,1,1);
        //
    button=gtk_button_new_with_label("Move Down");
    g_signal_connect(button,"clicked",G_CALLBACK(moveDown),window);
    gtk_grid_attach(GTK_GRID(grid), button, 25,2,2,1);
    
    button=gtk_button_new_with_label("Zoom In");
    g_signal_connect(button,"clicked",G_CALLBACK(zoomIn),window);
    gtk_grid_attach(GTK_GRID(grid), button, 25,4,1,1);
    
    button=gtk_button_new_with_label("Zoom Out");
    g_signal_connect(button,"clicked",G_CALLBACK(zoomOut),window);
    gtk_grid_attach(GTK_GRID(grid), button, 26,4,1,1);
    
    button=gtk_button_new_with_label("Transladar");
    g_signal_connect(button,"clicked",G_CALLBACK(translateObj),window);
    gtk_grid_attach(GTK_GRID(grid), button, 25,6,1,1);
    button=gtk_button_new_with_label("Escalonar");
    g_signal_connect(button,"clicked",G_CALLBACK(scaleObj),window);
    gtk_grid_attach(GTK_GRID(grid), button, 26,6,1,1);
    button=gtk_button_new_with_label("Rotacionar sobre o Centro");
    g_signal_connect(button,"clicked",G_CALLBACK(rotateByObjCenter),window);
    gtk_grid_attach(GTK_GRID(grid), button, 25,7,2,1);
    button=gtk_button_new_with_label("Rotacionar sobre a Origem");
    g_signal_connect(button,"clicked",G_CALLBACK(rotateByWorldCenter),window);
    gtk_grid_attach(GTK_GRID(grid), button, 25,8,2,1);
    button=gtk_button_new_with_label("Rotacionar sobre um Ponto");
    g_signal_connect(button,"clicked",G_CALLBACK(create_new_window),window);
    gtk_grid_attach(GTK_GRID(grid), button, 25,9,2,1);
    
    /* set a minimum size */
    gtk_widget_set_size_request (drawing_area, xvp, yvp);
    gtk_container_add (GTK_CONTAINER (frame), drawing_area);
    
    g_signal_connect (drawing_area, "draw",
                    G_CALLBACK (draw_cb), NULL);
    g_signal_connect (drawing_area,"configure-event",
                    G_CALLBACK (configure_event_cb), NULL);

  /* Ask to receive events the drawing area doesn't normally
   * subscribe to. In particular, we need to ask for the
   * button press and motion notify events that want to handle.
   */
 /* gtk_widget_set_events (drawing_area, gtk_widget_get_events (drawing_area)
                                     | GDK_BUTTON_PRESS_MASK
                                     | GDK_POINTER_MOTION_MASK);
   */ /* Make sure that everything, window and label, are visible */
    gtk_widget_show_all(window);

    gtk_main();

    return 0;
} 
