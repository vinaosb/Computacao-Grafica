#include <gtk/gtk.h>
#include <iostream>
#include <string>
#include <vector>
#include <math.h>
#include "polygon.cpp"
#include "clipping.cpp"
#include "window.cpp"
#include "viewport.cpp"
#include "descritorOBJ.cpp"
#include "Curva2D.cpp"
#include <iterator>

#define moveSpace 20
#define PI 3.14159265
#define FILE "poly.obj"

using namespace std;
//
//######## TO DO ########
//1-Corrigir ZoomIn e ZoomOut
//2-B-Spline deve aceitar qualquer quantidade de pontos.
//##########

static cairo_surface_t *surface = NULL;
GtkWidget *window;
GtkWidget *drawing_area;
GtkWidget *textview;

GtkWidget *gx1,*gx2,*gx3,*gy1,*gy2,*gy3,*name,*p_entry_x,*p_entry_y;
GtkWidget *p_entry_1,*p_entry_2,*p_entry_3,*p_entry_4,*p_entry_t,*radioBezier, *radioSpline;
GtkTextBuffer *buffer;
GtkTextMark *mark;
GtkTextIter iter;

Window win;
Viewport vp;

vector<Polygon> listPolygons,listPPC, listClip;

static void print_list(void)
{
    for (vector<Polygon>::iterator it=listPolygons.begin(); it != listPolygons.end(); ++it) {
        it->print();
    }
    cout << endl << endl << endl;
}

static void clear_surface (void)
{	 	  	 	    	   	      	   	 	    	    	 	
  cairo_t *cr;
  cr = cairo_create (surface);
  cairo_set_source_rgb (cr, 1, 1, 1);
  cairo_paint (cr);
  cairo_destroy (cr);
}
static void drawPoint(vector<Polygon>::iterator o)
{
  cairo_t *cr;
  cr = cairo_create (surface);  
  //cairo_set_line_width (cr, pointWidth);
  cairo_set_line_cap  (cr, CAIRO_LINE_CAP_ROUND); /* Round dot*/
    float xf = 0,yf = 0;
    for (vector<Polygon::point>::iterator it = o->getBeginIterator(); it != o->getEndIterator();++it) {
        xf = ((it->x - win.min.x)*(vp.max.x - vp.min.x)/((win.max.x-win.min.x)));
        yf = (1 - ((it->y - win.min.y)/((win.max.y-win.min.y))))*(vp.max.y - vp.min.y);
        cairo_move_to(cr, xf, yf);
        cairo_line_to(cr, xf, yf);
    }
  cairo_stroke(cr);
  gtk_widget_queue_draw (window);
}
static void drawPolygon(vector<Polygon>::iterator o)
{
    cairo_t *cr;
    cr = cairo_create (surface);  
    cairo_set_line_width (cr, vp.pointWidth);
    
    
    bool aux = false;
    int skip=0;
    float xf = 0,yf = 0, xf2 =0 , yf2=0;
    //cout<<"Draw Polygon" <<endl;
    for (vector<Polygon::point>::iterator it = o->getBeginIterator(); it != o->getEndIterator()-1;++it) {
       //cout <<"check "<<endl;
        xf = ((it->x - win.min.x)*(vp.max.x - vp.min.x)/((win.max.x-win.min.x)));
        yf = (1 - ((it->y - win.min.y)/((win.max.y-win.min.y))))*(vp.max.y - vp.min.y);
        xf2 = (((it+1)->x - win.min.x)*(vp.max.x - vp.min.x)/((win.max.x-win.min.x)));
        yf2 = (1 - (((it+1)->y - win.min.y)/((win.max.y-win.min.y))))*(vp.max.y - vp.min.y);
        if(skip++ % 2 != 0)
            continue;
        //skip++;
        
        // if (aux == false) {
        //     aux = true;
            cairo_move_to(cr, xf, yf);
            //cout <<"draw poly "<<xf<<","<<yf<<endl;
       // }
        cairo_set_line_cap  (cr, CAIRO_LINE_CAP_ROUND); /* Round dot*/
        cairo_line_to(cr, xf2, yf2);
        //cout <<"draw poly final "<<xf<<","<<yf<<endl;
    }
    
    // xf = ((o->getPoints().front().x - win.min.x)*(vp.max.x - vp.min.x)/((win.max.x-win.min.x)));
    // yf = (1 - ((o->getPoints().front().y - win.min.y)/((win.max.y-win.min.y))))*(vp.max.y - vp.min.y);
    // if(o->getType() != "Curve"){
    //     cairo_line_to(cr, xf, yf);
    //     cout <<"draw poly after "<<xf<<","<<yf<<endl;
    // }
    // cout << "getTypeC:"<<o->getType() <<endl;
    // cout << xf << " " << yf << endl;
  
    cairo_stroke(cr);
    gtk_widget_queue_draw (window);
}
static void drawArea(Polygon o)
{
    cairo_t *cr;
    cr = cairo_create (surface);  
    cairo_set_line_width (cr, 1);
    
    
    bool aux = false;
    float xf = 300,yf = 300;
    
    for (vector<Polygon::point>::iterator it = o.getBeginIterator(); it != o.getEndIterator();++it) {
        xf = ((it->x - win.min.x)*(vp.max.x - vp.min.x)/((win.max.x-win.min.x)));
        yf = (1 - ((it->y - win.min.y)/((win.max.y-win.min.y))))*(vp.max.y - vp.min.y);
        
        if (aux == false) {
            aux = true;
            cairo_move_to(cr, xf, yf);
        }
        cairo_set_line_cap  (cr, CAIRO_LINE_CAP_ROUND);
        cairo_line_to(cr, xf, yf);
    }
    
    xf = ((o.getPoints().front().x - win.min.x)*(vp.max.x - vp.min.x)/((win.max.x-win.min.x)));
    yf = (1 - ((o.getPoints().front().y - win.min.y)/((win.max.y-win.min.y))))*(vp.max.y - vp.min.y);
    cairo_line_to(cr, xf, yf);

    
    cairo_set_source_rgb (cr, 1, 0, 0);
    cairo_stroke(cr);
    gtk_widget_queue_draw (window);
}


static void redraw (void)
{  	 	    	    	 	
    clear_surface();
    
    Polygon obj("Tela");
    obj.addPoint(vp.min.x,vp.min.y);
    obj.addPoint(vp.max.x,vp.min.y);
    obj.addPoint(vp.max.x,vp.max.y);
    obj.addPoint(vp.min.x,vp.max.y);
    
    drawArea(obj);
//     cairo_t *cr; 

// 	cr = cairo_create(surface); 
// 	cairo_set_line_width(cr, 1); 
// 	cairo_move_to(cr, 9.5, 9.5); 
// 	cairo_line_to(cr, 370.5, 9.5); 
// 	cairo_line_to(cr, 370.5, 370.5); 
// 	cairo_line_to(cr, 9.5, 370.5); 
// 	cairo_line_to(cr, 9.5, 9.5); 
// 	cairo_set_source_rgb(cr, 1, 0, 0); 
// 	cairo_stroke(cr); 
// 	gtk_widget_queue_draw(window); 
    
    Clipping c(vp.min.x,vp.max.x,vp.min.y,vp.max.y);
    listClip.clear();
    vector<Polygon> temp = c.clip(listPPC);
    listClip.insert(listClip.end(), temp.begin(), temp.end());
    //cout << "listclip size: "<<listClip.size()<<endl;
    
    if (listClip.size() > 0)
        for (vector<Polygon>::iterator it = listClip.begin(); it != listClip.end(); ++it) {
            //cout << it->getPoints().at(0).x << endl;
            //cout << "ok" << endl;
            //cout << "getTypeIT:"<<it->getType() <<endl;
            if(it->getType()=="Point")
                drawPoint(it);
            else
                drawPolygon(it);
        }
    // if (listPPC.size() > 0)
    //     for (vector<Polygon>::iterator it = listPPC.begin(); it != listPPC.end(); ++it) {
    //         cout << it->getPoints().at(0).x << endl;
    //         cout << "ok" << endl;
    //         cout << "getTypeIT:"<<it->getType() <<endl;
    //         drawPolygon(it);
    //     }
}

static void move (int x, int y)
{
    for (vector<Polygon>::iterator it=listPPC.begin(); it != listPPC.end(); ++it) {
        it->translation(x,y);
    }
    redraw();
}

static void zoomIn () {
    win.zoom (TRUE);
    //vp.zoom (TRUE);
    redraw();
}

static void zoomOut() {
    win.zoom (FALSE);
    //vp.zoom (FALSE);
    redraw();
}

static void rotVP1() {
    for (vector<Polygon>::iterator it=listPPC.begin(); it != listPPC.end(); ++it) {
        it->translation(-(win.max.x-win.min.x)/2,-(win.max.y-win.min.y)/2);
        it->rotation(10);
        it->translation((win.max.x-win.min.x)/2,(win.max.y-win.min.y)/2);
    }
    redraw();
}

static void rotVP2() {
    for (vector<Polygon>::iterator it=listPPC.begin(); it != listPPC.end(); ++it) {
        it->translation(-(win.max.x-win.min.x)/2,-(win.max.y-win.min.y)/2);
        it->rotation(-10);
        it->translation((win.max.x-win.min.x)/2,(win.max.y-win.min.y)/2);
    }
    redraw();
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

static void insert_text_buffer(Polygon obj){
  string text = obj.getName() + " - " + obj.getType();
  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview));
  mark = gtk_text_buffer_get_insert (buffer);
  gtk_text_buffer_get_iter_at_mark (buffer, &iter, mark);
  if (gtk_text_buffer_get_char_count(buffer))
    gtk_text_buffer_insert (buffer, &iter, "\n", 1);
  gtk_text_buffer_insert (buffer, &iter, text.c_str(), -1);
}

static void btn_point_clicked_cb(){
    string tname(gtk_entry_get_text(GTK_ENTRY(name)));
    string tx1(gtk_entry_get_text(GTK_ENTRY(gx1)));
    string ty1(gtk_entry_get_text(GTK_ENTRY(gy1)));
  
    Polygon obj(tname);
    obj.addPoint(stof(tx1),stof(ty1));
  
    listPolygons.push_back(obj);
    listPPC.push_back(obj);

    //insere objeto no TextView
    insert_text_buffer(obj);
    redraw();
}	 	  	 	    	   	      	   	 	    	    	 	
static void btn_line_clicked_cb(){
    string tname(gtk_entry_get_text(GTK_ENTRY(name)));
    string tx1(gtk_entry_get_text(GTK_ENTRY(gx1)));
    string tx2(gtk_entry_get_text(GTK_ENTRY(gx2)));
    string ty1(gtk_entry_get_text(GTK_ENTRY(gy1)));
    string ty2(gtk_entry_get_text(GTK_ENTRY(gy2)));
  
    Polygon obj(tname);
    obj.addPoint(stof(tx1),stof(ty1));
    obj.addPoint(stof(tx2),stof(ty2));
    obj.setType("Line"); 
  
    listPolygons.push_back(obj);
    listPPC.push_back(obj);
  
    //insere objeto no TextView
    insert_text_buffer(obj);
  
    redraw();
    //draw_line(obj.x1,obj.x2,obj.y1,obj.y2);
 } 
static void btn_triangle_clicked_cb(){
    string tname(gtk_entry_get_text(GTK_ENTRY(name)));
    string tx1(gtk_entry_get_text(GTK_ENTRY(gx1)));
    string tx2(gtk_entry_get_text(GTK_ENTRY(gx2)));
    string tx3(gtk_entry_get_text(GTK_ENTRY(gx3)));
    string ty1(gtk_entry_get_text(GTK_ENTRY(gy1)));
    string ty2(gtk_entry_get_text(GTK_ENTRY(gy2)));
    string ty3(gtk_entry_get_text(GTK_ENTRY(gy3)));
   
    Polygon obj(tname);
    obj.addPoint(stof(tx1),stof(ty1));
    obj.addPoint(stof(tx2),stof(ty2));
    obj.addPoint(stof(tx3),stof(ty3));
    obj.setType("Triangle"); 
   
    listPolygons.push_back(obj);
    listPPC.push_back(obj);
  
    //insere objeto no TextView
    insert_text_buffer(obj);
   
    redraw();
  }
static vector<int> splitCoord(string entry_ponto){
    vector<int> ponto;
    string x,y;
    size_t pos = 0;
    string token;
    pos = entry_ponto.find(",");
    x = entry_ponto.substr(0, pos);
    entry_ponto.erase(0, pos + 1);
    y = entry_ponto;
    ponto.push_back(atoi(x.c_str()));
    ponto[1] = atoi(y.c_str());
 
    return ponto;
}
static void btn_draw_curva2D(){
    string tname="Curva2D";
    Polygon obj(tname);
    string tp1(gtk_entry_get_text(GTK_ENTRY(p_entry_1)));
    string tp2(gtk_entry_get_text(GTK_ENTRY(p_entry_2)));
    string tp3(gtk_entry_get_text(GTK_ENTRY(p_entry_3)));
    string tp4(gtk_entry_get_text(GTK_ENTRY(p_entry_4)));
    string t(gtk_entry_get_text(GTK_ENTRY(p_entry_t)));
    vector<int> p;
    p = splitCoord(tp1);
    obj.addPoint(p[0],p[1]);
    p = splitCoord(tp2);
    obj.addPoint(p[0],p[1]);
    p = splitCoord(tp3);
    obj.addPoint(p[0],p[1]);
    p = splitCoord(tp4);
    obj.addPoint(p[0],p[1]);
    
    // obj.addPoint(100,100);
    // obj.addPoint(200,200);
    // obj.addPoint(300,200);
    // obj.addPoint(400,100);
    
    Curva2D curva;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radioBezier)))
        obj = curva.getPointsBezier(obj, stof(t));
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radioSpline)))
        obj = curva.getPointsBSpline(obj, stof(t));
    
    obj.setType("Curve");
    obj.setName(tname);
    
    listPolygons.push_back(obj);
    listPPC.push_back(obj);
    //insere objeto no TextView
    insert_text_buffer(obj);
    obj.print();
   
    redraw();
}
  
static Polygon* getPolygon(){
    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview));
    mark = gtk_text_buffer_get_selection_bound (buffer);
    gtk_text_buffer_get_iter_at_mark (buffer, &iter, mark);
    int linha_cursor = gtk_text_iter_get_line (&iter);
    Polygon *o = &listPPC.at(linha_cursor);
    return o;
}
static void translateObj(){
    Polygon *o = getPolygon();
    o->translation(10,10);
    redraw();
}
static void scaleObj(){
    Polygon *o = getPolygon();
    o->scaling(1.1);
    redraw();
}
static void rotateByObjCenter(){
    Polygon *o = getPolygon();
    Polygon::point center = o->findCenter();
    o->translation(-center.x,-center.y);
    o->rotation(30);
    o->translation(center.x,center.y);
    redraw();
 };
static void rotateByWorldCenter(){
    Polygon *o = getPolygon();
    o->rotation(30);
    redraw();
 };
static void rotateByAnyPoint(){
    Polygon *o = getPolygon();
    string px(gtk_entry_get_text(GTK_ENTRY(p_entry_x)));
    string py(gtk_entry_get_text(GTK_ENTRY(p_entry_y)));
    Polygon::point p;
    p.x = stof(px);
    p.y = stof(py);
    o->translation(-p.x,-p.y);
    o->rotation(30);
    o->translation(p.x,p.y);
    redraw();
};
static void btn_clear_clicked_cb(){	 	 
    listPPC.clear();
    // apaga buffer do textView
    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview));
    gtk_text_buffer_set_text(buffer,"",-1);
    
    clear_surface();
    //vp.reset();
    gtk_widget_queue_draw (window);
    while(!listPolygons.empty())
        listPolygons.pop_back();
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
void window_curva2D(){
    GtkWidget *p_window, *p_label_indice, *p_label_t;
    GtkWidget *grid, *p_label_1,*p_label_2,*p_label_3,*p_label_4;
    GtkWidget *p_label_main,*p_button,*p_button_cancel, *space;

    p_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_position(GTK_WINDOW(p_window), GTK_WIN_POS_CENTER);
    gtk_window_set_title(GTK_WINDOW(p_window), "Curva 2D");
    gtk_window_set_default_size(GTK_WINDOW(p_window), 300, 200);

    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(p_window), grid);
    
    radioBezier = gtk_radio_button_new_with_label(NULL,"Bezier");
    radioSpline = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radioBezier),"B-Spline");
    p_label_main = gtk_label_new("ex: P1:< 100,100 >");
    space = gtk_label_new("");
    gtk_grid_attach(GTK_GRID(grid), radioBezier,0,0,1,1);
    gtk_grid_attach(GTK_GRID(grid), radioSpline,1,0,1,1);
    gtk_grid_attach(GTK_GRID(grid), p_label_main,0,1,1,1);
    p_label_1 = gtk_label_new("P1: ");
    gtk_grid_attach(GTK_GRID(grid),p_label_1,0,2,1,2);
    p_entry_1 = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), p_entry_1,1,2,1,2);
    p_label_2= gtk_label_new("P2: ");
    gtk_grid_attach(GTK_GRID(grid), p_label_2,0,4,1,2);
    p_entry_2 = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), p_entry_2,1,4,1,2);
    p_label_3= gtk_label_new("P3: ");
    gtk_grid_attach(GTK_GRID(grid), p_label_3,0,6,1,2);
    p_entry_3 = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), p_entry_3,1,6,1,2);
    p_label_4= gtk_label_new("P4: ");
    gtk_grid_attach(GTK_GRID(grid), p_label_4,0,8,1,2);
    p_entry_4 = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), p_entry_4,1,8,1,2);
    
    p_label_indice = gtk_label_new("Indice de Parametrização");
    gtk_grid_attach(GTK_GRID(grid), p_label_indice,1,10,1,1);
    p_label_t = gtk_label_new("t = ");
    gtk_grid_attach(GTK_GRID(grid), p_label_t,0,11,1,1);
    p_entry_t = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), p_entry_t,1,11,1,1);

    gtk_grid_attach(GTK_GRID(grid), space,1,12,1,1);
    p_button = gtk_button_new_with_label("Draw");
    gtk_grid_attach(GTK_GRID(grid), p_button,1,13,1,1);
    p_button_cancel = gtk_button_new_with_label("Cancel");
    gtk_grid_attach(GTK_GRID(grid), p_button_cancel,3,13,1,1);
    
    gtk_entry_set_text(GTK_ENTRY(p_entry_1),"15,50");
    gtk_entry_set_text(GTK_ENTRY(p_entry_2),"100,400");
    gtk_entry_set_text(GTK_ENTRY(p_entry_3),"200,200");
    gtk_entry_set_text(GTK_ENTRY(p_entry_4),"450,350");
    gtk_entry_set_text(GTK_ENTRY(p_entry_t),"0.1");
    
    g_signal_connect(p_button,"clicked",G_CALLBACK(btn_draw_curva2D),p_window);
    g_signal_connect_swapped(p_button,"clicked",G_CALLBACK(gtk_widget_destroy),p_window);
    g_signal_connect_swapped(p_button_cancel,"clicked",G_CALLBACK(gtk_widget_destroy),p_window);

    gtk_widget_show_all(p_window);
}

    void moveUp(){
        win.moveUp();
        vp.moveUp();
        redraw();
    }
    void moveLeft (){
        win.moveLeft();
        vp.moveLeft();
        redraw();
    }
    
    void moveRight (){
        win.moveRight();
        vp.moveRight();
        redraw();
    }
    
    void moveDown (){
        win.moveDown();
        vp.moveDown();
        redraw();
    }
    
    void load() {
        DescritorOBJ obj(FILE);
        
        vector<Polygon> aux = obj.loader();
    }
    
    void save() {
        DescritorOBJ obj(FILE);
        
        obj.clearData();
        
        for (vector<Polygon>::iterator it = listPolygons.begin(); it != listPolygons.end(); ++it) {
            obj.saver(*it);
        }
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

    label = gtk_label_new("Polygons");
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
    g_signal_connect(button,"clicked",G_CALLBACK(btn_triangle_clicked_cb),window);
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
    
    button=gtk_button_new_with_label("Curva 2D");
    g_signal_connect(button,"clicked",G_CALLBACK(window_curva2D),window);
    gtk_grid_attach(GTK_GRID(grid), button, 0,1,1,1);
    
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
        //
    button=gtk_button_new_with_label("Rotate VP <<");
    g_signal_connect(button,"clicked",G_CALLBACK(rotVP1),window);
    gtk_grid_attach(GTK_GRID(grid), button, 25,3,1,1);
        //
    button=gtk_button_new_with_label("Rotate VP >>");
    g_signal_connect(button,"clicked",G_CALLBACK(rotVP2),window);
    gtk_grid_attach(GTK_GRID(grid), button, 26,3,1,1);
    
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
    
    
    button=gtk_button_new_with_label("Load Obj");
    g_signal_connect(button,"clicked",G_CALLBACK(load),window);
    gtk_grid_attach(GTK_GRID(grid), button, 25,12,1,1);
    button=gtk_button_new_with_label("Save Polygons");
    g_signal_connect(button,"clicked",G_CALLBACK(save),window);
    gtk_grid_attach(GTK_GRID(grid), button, 26,12,1,1);
    
    /* set a minimum size */
    gtk_widget_set_size_request (drawing_area, vp.max.x-vp.min.x, vp.max.y-vp.min.y);
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
