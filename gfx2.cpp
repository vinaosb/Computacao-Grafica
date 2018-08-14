#include <gtk/gtk.h>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <string>
#include <list>

#define moveSpace 50

static cairo_surface_t *surface = NULL;
GtkWidget *window;
GtkWidget *drawing_area;
GtkWidget *textview;

GtkWidget *x1,*x2,*y1,*y2,*name;
GtkTextBuffer *buffer;
GtkTextMark *mark;
GtkTextIter iter;

typedef struct {
    //tipo 0 = ponto, 1 = linha, 2 = retangulo
    double x1,x2,y1,y2;
    std::string name;
    int tipo = 0;
} object;

static int xvp = 400, yvp = 400, xwmin = 0, xwmax = 400, ywmin = 0, ywmax = 400, pointWidth = 4;

static double zoom = 1;

std::list<object> listObjects;

static void print_list(void)
{
    for (std::list<object>::iterator it=listObjects.begin(); it != listObjects.end(); ++it) {
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

static void draw_rectangle(double x1, double x2, double y1, double y2) 
{
   cairo_t *cr;
   cr = cairo_create (surface);
   
   double xf1 = ((x1 - xwmin)*(xvp)/((xwmax-xwmin)*zoom));
   double xf2 = ((x2 - xwmin)*(xvp)/((xwmax-xwmin)*zoom));
   double yf1 = (1 - ((y1 - ywmin)/((ywmax-ywmin)*zoom)))*(yvp);
   double yf2 = (1 - ((y2 - ywmin)/((ywmax-ywmin)*zoom)))*(yvp);
   
   cairo_move_to(cr, xf1, yf1);
   cairo_line_to(cr, xf1, yf2);
   cairo_line_to(cr, xf2, yf2);
   cairo_line_to(cr, xf2, yf1);
   cairo_line_to(cr, xf1, yf1);
   cairo_stroke(cr);
   gtk_widget_queue_draw (window);
}

static void redraw (void)
{  	 	    	    	 	
    clear_surface();
    
    for (std::list<object>::iterator it=listObjects.begin(); it != listObjects.end(); ++it) {
        switch (it->tipo) {
            case 1 :
                draw_line(it->x1,it->x2,it->y1,it->y2);
                break;
            case 2 :
                draw_rectangle(it->x1,it->x2,it->y1,it->y2);
                break;
            default :
                draw_point(it->x1,it->y1);
        }
    }
}

static void move (int x, int y)
{
    for (std::list<object>::iterator it=listObjects.begin(); it != listObjects.end(); ++it) {
        it->x1 = it->x1 + x;
        it->x2 = it->x2 + x;
        it->y1 = it->y1 + y;
        it->y2 = it->y2 + y;
    }
    redraw();
}
static void moveUp(){
    move(0,-moveSpace);
}
static void moveLeft (){
    move(moveSpace,0);
}

static void moveRight (){
    move(-moveSpace,0);
}

static void moveDown (){
    move(0,moveSpace);
}

static void zoomWindow (bool z)
{
    if (!z)  {
        zoom = zoom + 0.1;
        pointWidth--;
    }
    else {
        zoom = zoom - 0.1;
        pointWidth++;
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
            tipo = "Retangulo";
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
  std::string tx1(gtk_entry_get_text(GTK_ENTRY(x1)));
  std::string ty1(gtk_entry_get_text(GTK_ENTRY(y1)));
  
  object obj;
  obj.x1 = stod(tx1);
  obj.x2 = stod(tx1);
  obj.y1 = stod(ty1);
  obj.y2 = stod(ty1);
  
  obj.name = tname;
  obj.tipo = 0;
  listObjects.push_back(obj);

  //insere objeto no TextView
  insert_text_buffer(obj);
  redraw();
}	 	  	 	    	   	      	   	 	    	    	 	
static void btn_line_clicked_cb(){
  std::string tname(gtk_entry_get_text(GTK_ENTRY(name)));
  std::string tx1(gtk_entry_get_text(GTK_ENTRY(x1)));
  std::string tx2(gtk_entry_get_text(GTK_ENTRY(x2)));
  std::string ty1(gtk_entry_get_text(GTK_ENTRY(y1)));
  std::string ty2(gtk_entry_get_text(GTK_ENTRY(y2)));
  
  object obj;
  obj.x1 = stod(tx1);
  obj.x2 = stod(tx2);
  obj.y1 = stod(ty1);
  obj.y2 = stod(ty2);
  
  
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
   std::string tx1(gtk_entry_get_text(GTK_ENTRY(x1)));
   std::string tx2(gtk_entry_get_text(GTK_ENTRY(x2)));
   std::string ty1(gtk_entry_get_text(GTK_ENTRY(y1)));
   std::string ty2(gtk_entry_get_text(GTK_ENTRY(y2)));
   
   
   object obj;
   obj.x1 = stod(tx1);
   obj.x2 = stod(tx2);
   obj.y1 = stod(ty1);
   obj.y2 = stod(ty2);
   
   obj.name = tname;
   obj.tipo = 2;
   listObjects.push_back(obj);
  
   //insere objeto no TextView
   insert_text_buffer(obj);
   
   redraw();
  
  }
  static void translateObj(){
      
  }
  static void scaleObj(){
      
  }
 static void rotateByObjCenter(){};
 static void rotateByWorldCenter(){};
 static void rotateByAnyPoint(){};
 
 static void btn_clear_clicked_cb(){	 	  	 	    	   	      	   	 	    	    	 	
    //print_list();//print para debug
    // apaga buffer do textView
    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview));
    gtk_text_buffer_set_text(buffer,"",-1);
    
    clear_surface();
    gtk_widget_queue_draw (window);
    while(!listObjects.empty())
        listObjects.pop_front();
        
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

    /*
    ** Map the destroy signal of the window to gtk_main_quit;
    ** When the window is about to be destroyed, we get a notification and
    ** stop the main GTK+ loop by returning 0
    */
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    /*
    ** Assign the variable "label" to a new GTK label,
    ** with the text "Hello, world!"
    */
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

    label = gtk_label_new("Objects");
    gtk_grid_attach(GTK_GRID(grid), label, 0,8,1,1);
    
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
    x1=gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(x1),"0");
    gtk_grid_attach(GTK_GRID(grid), x1, 1,4,2,1);
    y1=gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(y1),"0");
    gtk_grid_attach(GTK_GRID(grid), y1, 1,5,2,1);
    x2=gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(x2),"0");
    gtk_grid_attach(GTK_GRID(grid), x2, 1,6,2,1);
    y2=gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(y2),"0");
    gtk_grid_attach(GTK_GRID(grid), y2, 1,7,2,1);
    
    button=gtk_button_new_with_label("Point");
    g_signal_connect(button,"clicked",G_CALLBACK(btn_point_clicked_cb),window);
    gtk_grid_attach(GTK_GRID(grid), button, 0,2,1,1);

    button=gtk_button_new_with_label("Line");
    g_signal_connect(button,"clicked",G_CALLBACK(btn_line_clicked_cb),window);
    gtk_grid_attach(GTK_GRID(grid), button, 1,2,1,1);

    button=gtk_button_new_with_label("Rectangle");
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
    gtk_grid_attach(GTK_GRID(grid), vbox, 0,9,3,8);
    
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
    g_signal_connect(button,"clicked",G_CALLBACK(rotateByAnyPoint),window);
    gtk_grid_attach(GTK_GRID(grid), button, 25,9,2,1);
    
    /* set a minimum size */
    gtk_widget_set_size_request (drawing_area, xvp, yvp);
    gtk_container_add (GTK_CONTAINER (frame), drawing_area);
    
    g_signal_connect (drawing_area, "draw",
                    G_CALLBACK (draw_cb), NULL);
    g_signal_connect (drawing_area,"configure-event",
                    G_CALLBACK (configure_event_cb), NULL);
      /* Event signals */

  /* Ask to receive events the drawing area doesn't normally
   * subscribe to. In particular, we need to ask for the
   * button press and motion notify events that want to handle.
   */
 /* gtk_widget_set_events (drawing_area, gtk_widget_get_events (drawing_area)
                                     | GDK_BUTTON_PRESS_MASK
                                     | GDK_POINTER_MOTION_MASK);
   */ /* Make sure that everything, window and label, are visible */
    
    gtk_widget_show_all(window);

    /*
    ** Start the main loop, and do nothing (block) until
    ** the application is closed
    */
    gtk_main();

    return 0;
}	 	  	 	    	   	      	   	 	    	    	 	
