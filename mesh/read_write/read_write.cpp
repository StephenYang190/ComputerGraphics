///////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include<string>
#include "read_write.h"
namespace MeshN {
	///////////////////////////////////////////////////////////////////////////////
	// implementation
	///////////////////////////////////////////////////////////////////////////////
	template <class Mesh>
	void ReaderWriterT<Mesh>::set_mesh(Mesh* _ptr_mesh) {
		mesh_ = _ptr_mesh;
	}

	///////////////////////////////////////////////////////////////////////////////
	template <class Mesh>
	bool ReaderWriterT<Mesh>::off_reader(const char* _filename) { 
		if ( mesh_ == NULL ) { 
			std::cout<<"\nPointer to mesh is NOT defined!\n";
			exit(0);
		}

		FILE *modelfile;

		if ((modelfile = fopen( _filename, "r") ) == NULL) {
			std::cout<<"\nError: Can't open the model file!\n";
			return false;  
		}

		char letter1 = ' ',letter2=' ',letter3=' ';
		for ( ; ; ) {             // skip over comments and blank 
			if (feof(modelfile)) { // no data in file
				std::cout<<"\nError: No data in model file!\n";
				return false;  
			}

			fscanf(modelfile, "%c %c %c", &letter1,&letter2,&letter3);
			if ((letter1 == 'O')&&(letter2=='F')&&(letter3=='F')) break;  //OK!!
		} 

		int no_vertex,no_facet,no_edge;
		//std::cout<<no_vertex;

		fscanf(modelfile,"%d %d %d",&no_vertex,&no_facet,&no_edge);
		//std::cout<<no_vertex;


		float  x,  y,  z;   // components of coordinats 
		int    p0, p1, p2,p3;  // index of points
		for(int i=0;i<no_vertex;i++)

		{fscanf(modelfile, "%f %f %f", &x, &y, &z);
		mesh_->add_vertex( Coord(x, y, z) );
		}

		for(int i=0;i<no_facet;i++)

		{ fscanf(modelfile, "%d %d %d %d", &p0, &p1, &p2,&p3); //// ????????

		//mesh_->add_facet( VertexHandle(p0-1), VertexHandle(p1-1), VertexHandle(p2-1));
		mesh_->add_facet( VertexHandle(p1), VertexHandle(p2), VertexHandle(p3));

		}  // end of while

		fclose(modelfile);  

		return true;
	}


	///////////////////////////////////////////////////////////////////////////// 
	template <class Mesh>
	bool ReaderWriterT<Mesh>::ogl_writer(bool _orient, bool _smooth){
		HalfedgeHandle       cshh;
		Mesh::FacetIterator  fit (mesh_->facet_begin()); 

			//glShadeModel(GL_FLAT);
		    glShadeModel(GL_SMOOTH);
			int orient = true;// (_orient) ? 1 : -1; 
			mesh_->update_normals(); 

			for ( ; fit != mesh_->facet_end(); ++fit ) {	

				if ( (*fit).status_.is_deleted() ) continue;  

				cshh = fit->halfedge_handle_;
				FacetHandle fh = mesh_->facet_handle(cshh);
				const VertexHandle& vh0 = mesh_->vertex_handle(cshh);
				glBegin(GL_TRIANGLES); 
				do {
					const VertexHandle& vh = mesh_->vertex_handle(cshh);

					glNormal3fv( mesh_->normal(fh)*orient );
					glVertex3fv( mesh_->coord(vh) );
					cshh = mesh_->next_halfedge_handle(cshh);
				} while ( cshh != fit->halfedge_handle_ );
				glEnd();
			}

		return true;
	}


	///////////////////////////////////////////////////////////////////////////// 
	template <class Mesh>
	bool ReaderWriterT<Mesh>::ogl_writer2(bool _orient, bool _smooth){////在里面把三角面片法向画出
		// HalfedgeHandle       cshh;
		// Mesh::FacetIterator  fit (mesh_->facet_begin()); 

		// //glShadeModel(GL_FLAT);
		// glShadeModel(GL_SMOOTH);
		// int orient = true;// (_orient) ? 1 : -1; 
		// mesh_->update_normals(); 

		// for ( ; fit != mesh_->facet_end(); ++fit ) {	

		// 	if ( (*fit).status_.is_deleted() ) continue;  

		// 	cshh = fit->halfedge_handle_;
		// 	FacetHandle fh = mesh_->facet_handle(cshh);
		// 	Mesh::Normal fct_norm = fit->normal_;
		// 	Mesh::Coord fct_centr = fit->centroid_;

		// 	glBegin(GL_LINES); 
		// 	glVertex3fv(fct_centr);
		// 	glVertex3fv(fct_centr + fct_norm);
		// 	glEnd();
		// }

		// return true;
		HalfedgeHandle       cshh;
		Mesh::FacetIterator  fit (mesh_->facet_begin()); 

			//glShadeModel(GL_FLAT);
		    glShadeModel(GL_SMOOTH);
			int orient = true;// (_orient) ? 1 : -1; 
			mesh_->update_normals(); 

			for ( ; fit != mesh_->facet_end(); ++fit ) {	

				if ( (*fit).status_.is_deleted() ) continue;  

				cshh = fit->halfedge_handle_;
				FacetHandle fh = mesh_->facet_handle(cshh);
				Mesh::Normal fct_norm = fit->normal_;

				//calculate the centroid
				HalfedgeHandle& hh = mesh_->halfedge_handle(fh);
				HalfedgeHandle& n_hh = mesh_->next_halfedge_handle(hh);
				HalfedgeHandle& pre_hh = mesh_->prev_halfedge_handle(hh);

				VertexHandle& vh1 = mesh_->vertex_handle(hh);
				VertexHandle& vh2 = mesh_->vertex_handle(n_hh);
				VertexHandle& vh3 = mesh_->vertex_handle(pre_hh);

				Mesh::Coord fct_centr = (mesh_->coord(vh1) + mesh_->coord(vh2)+ mesh_->coord(vh3) )/3.0;
			
				const VertexHandle& vh0 = mesh_->vertex_handle(cshh);
				glBegin(GL_TRIANGLES); 
				do {
					const VertexHandle& vh = mesh_->vertex_handle(cshh);

					glNormal3fv( mesh_->normal(fh)*orient );
					glVertex3fv( mesh_->coord(vh) );
					cshh = mesh_->next_halfedge_handle(cshh);
				} while ( cshh != fit->halfedge_handle_ );
				glEnd();

				glBegin(GL_LINES); 
				glVertex3fv(fct_centr);
				glVertex3fv(fct_centr + fct_norm);
				float ff = fct_norm[0] * fct_norm[0] + fct_norm[1] * fct_norm[1] + fct_norm[2] * fct_norm[2];
				std::cout << ff << std::endl;
				glEnd();
			}

		return true;
	}
	
} //namespace
