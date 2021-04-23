#include "point.h"
#include "tetra_meas.h"
#include "pcr_adv.h"
#include "c_rtree.h"
#include "ProgressBar.hpp"
#include "util.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdio>
#include <vector>
#include <unordered_map>
#include <string>
#include <ctime>
#include <cstdlib>

using namespace std;

/*
struct Entry_Stat_3nn {
    int global_id;
    Entry_Stat_3nn(int id) {
        global_id = id;
    }

    int num_discarded;
    double nn_dist[3];
};

struct Entry_Stat_3lnn {
    int global_id;
    Entry_Stat_3lnn(int id) {
        global_id = id;
        for (int i = 0; i < 3; i++) {
            num_discarded[i] = 0;
            nn_dist[i] = 0.0;
        }
    }

    int num_discarded[3];
    double nn_dist[3];
};

struct Entry_Stat {
    int global_id;
    Entry_Stat(int id) {
        global_id = id;
    }
    
    double nn_dist[3];
};
*/

bool verify_angle_3nn(vector<Pt3D>& accepted_pa, Pt3D* pb, double cos_min_ang) {

    for (auto &pa: accepted_pa) {
        double ct = cos_theta(&pa, pb);
        if (ct > cos_min_ang) {
            return false;
        }
    }
    return true;

}

bool verify_angle_3lnn(Pt3D* pa, Pt3D* pb, double cos_min_ang) {

    double ct = cos_theta(pa, pb);
    if (ct > cos_min_ang) {
        return false;
    } else {
        return true;
    }

}

void cal_3nn_entry_nonsim(PtwID* p, double min, double cos_min_ang, Mesh* mesh_p, C_RTree* r_p, bool debug_mode, Entry* prem_entry /*, Entry_Stat_3nn& es*/) {

    const int TENT = 40;

    int nn[TENT];
    r_p->nn_sphere(p->pt, min, nn, {}, TENT);

    PtwID a, b, c;

    int accepted = 0, discarded = 0;
    vector<Pt3D> accepted_pa;

    if (nn[0] < 0 || nn[0] >= mesh_p->size()) {
        return;
    }
    a = PtwID(nn[0], mesh_p);
    accepted++;
    accepted_pa.push_back(*a.pt - *p->pt);

    for (int i = 1; i < TENT; i++) {
        if (accepted >= 3) {
            break;
        }
        if (nn[i] < 0 || nn[i] >= mesh_p->size()) {
            return;
        }
        PtwID t = PtwID(nn[i], mesh_p);
        auto pt = *t.pt - *p->pt;
        if (verify_angle_3nn(accepted_pa, &pt, cos_min_ang)) {
            switch (accepted) {
            case 1:
                b = t;
                accepted++;
                accepted_pa.push_back(pt);
                break;
            case 2:
                c = t;
                accepted++;
                accepted_pa.push_back(pt);
                break;
            default:
                break;
            }
        } else {
            discarded++;
        }
    }

    // cout << "# accepted: " << accepted << endl;
    // cout << "# discarded: " << discarded << endl;

    // cout << "pa = " << accepted_pa[0].mode() << endl;
    // cout << "pb = " << accepted_pa[1].mode() << endl;
    // cout << "pc = " << accepted_pa[2].mode() << endl;

    // auto cos_a0pa1 = dot_prd(&pa0, &pa1) / (pa0_mode * pa1_mode);
    // auto cos_a0pa2 = dot_prd(&pa0, &pa2) / (pa0_mode * pa2_mode);
    // auto cos_a1pa2 = dot_prd(&pa1, &pa2) / (pa1_mode * pa2_mode);

    // cout << "cos_a0pa1 = " << cos_a0pa1 << endl;
    // cout << "cos_a0pa2 = " << cos_a0pa2 << endl;
    // cout << "cos_a1pa2 = " << cos_a1pa2 << endl;


    if (accepted >= 3) {
        // get the ratio set
        auto ratio_set = get_ratio_set_vol(p->pt, a.pt, b.pt, c.pt);
        prem_entry->set(p, &a, &b, &c, ratio_set.volume, ratio_set.ratio);
        prem_entry->fail = false;
    }

    // es.num_discarded = discarded;
    // for (int i = 0; i < 3; i++) {
    //     if (i < accepted_pa.size()) {
    //         es.nn_dist[i] = accepted_pa[i].mode();
    //     } else {
    //         es.nn_dist[i] = 0;
    //     }
    // }

}

void cal_3nn_entry_sim(PtwID* p, double min, Mesh* mesh_p, C_RTree* r_p, bool debug_mode, Entry* prem_entry /*, Entry_Stat_3nn& es*/) {

    int nn[3];
    r_p->nn_sphere(p->pt, min, nn, {}, 3);

    PtwID a, b, c;
    a = PtwID(nn[0], mesh_p);
    b = PtwID(nn[1], mesh_p);
    c = PtwID(nn[2], mesh_p);

    // get the ratio set
    auto ratio_set = get_ratio_set_vol(p->pt, a.pt, b.pt, c.pt);
    prem_entry->set(p, &a, &b, &c, ratio_set.volume, ratio_set.ratio);
    prem_entry->fail = false;

}

void cal_3nn_entry(PtwID* p, double min, double cos_min_ang, Mesh* mesh_p, C_RTree* r_p, bool debug_mode, bool simple, Entry* prem_entry) {

    if (simple) {
        cal_3nn_entry_sim(p, min, mesh_p, r_p, debug_mode, prem_entry);
    } else {
        cal_3nn_entry_nonsim(p, min, cos_min_ang, mesh_p, r_p, debug_mode, prem_entry);
    }

}

// order: max -> mid -> min (c -> b -> a)
void cal_3lnn_entry_nonsim(PtwID* p, double radius, double radius_1, double radius_2, double cos_min_ang, Mesh* mesh_p, C_RTree* r_p, bool debug_mode, Entry* prem_entry /*, Entry_Stat_3lnn& es*/) {

    int nn_c;
    r_p->nn_sphere(p->pt, radius_2, &nn_c);

    PtwID a, b, c;
    c = PtwID(nn_c, mesh_p);
    auto pc = *c.pt - *p->pt;

    const int TENT = 20;
    
    bool got_b = false;
    int nn_b[TENT];
    r_p->nn_sphere(p->pt, radius_1, nn_b, {}, TENT);
    for (int i = 0; i < TENT; i++) {
        PtwID t = PtwID(nn_b[i], mesh_p);
        auto pt = *t.pt - *p->pt;
        if (pt.mode() >= radius_2) {
            break;
        }
        if (verify_angle_3lnn(&pc, &pt, cos_min_ang)) {
            b = t;
            got_b = true;
            break;
        // } else {
        //     es.num_discarded[1]++;
        }
    }
    if (!got_b) {
        return;
    }

    auto pb = *b.pt - *p->pt;
    
    bool got_a = false;
    int nn_a[TENT];
    r_p->nn_sphere(p->pt, radius, nn_a, {}, TENT);
    for (int i = 0; i < TENT; i++) {
        PtwID t = PtwID(nn_a[i], mesh_p);
        auto pt = *t.pt - *p->pt;
        if (pt.mode() >= radius_1) {
            break;
        }
        if (verify_angle_3lnn(&pc, &pt, cos_min_ang) && verify_angle_3lnn(&pb, &pt, cos_min_ang)) {
            a = t;
            got_a = true;
            break;
        // } else {
        //     es.num_discarded[0]++;
        }
    }
    if (!got_a) {
        return;
    }

    // get the ratio set
    auto ratio_set = get_ratio_set_vol(p->pt, a.pt, b.pt, c.pt);
    prem_entry->set(p, &a, &b, &c, ratio_set.volume, ratio_set.ratio);
    prem_entry->fail = false;

}

// order: min -> mid -> max
void cal_3lnn_entry_sim(PtwID* p, double radius, double radius_1, double radius_2, Mesh* mesh_p, C_RTree* r_p, bool debug_mode, Entry* prem_entry /*, Entry_Stat_3lnn& es*/) {

    int nn[3];
    r_p->nn_sphere(p->pt, radius, &nn[0]);
    r_p->nn_sphere(p->pt, radius_1, &nn[1]);
    r_p->nn_sphere(p->pt, radius_2, &nn[2]);

    if (nn[0] == nn[1] || nn[0] == nn[2] || nn[1] == nn[2]) {
        // cout << "fail triggered" << endl;
        return;
    }

    PtwID a, b, c;
    a = PtwID(nn[0], mesh_p);
    b = PtwID(nn[1], mesh_p);
    c = PtwID(nn[2], mesh_p);

    // get the ratio set
    auto ratio_set = get_ratio_set_vol(p->pt, a.pt, b.pt, c.pt);
    prem_entry->set(p, &a, &b, &c, ratio_set.volume, ratio_set.ratio);
    prem_entry->fail = false;

}

void cal_3lnn_entry(PtwID* p, double radius, double radius_1, double radius_2, double cos_min_ang, Mesh* mesh_p, C_RTree* r_p, bool debug_mode, bool simple, Entry* prem_entry) {

    if (simple) {
        cal_3lnn_entry_sim(p, radius, radius_1, radius_2, mesh_p, r_p, debug_mode, prem_entry);
    } else {
        cal_3lnn_entry_nonsim(p, radius, radius_1, radius_2, cos_min_ang, mesh_p, r_p, debug_mode, prem_entry);
    }

}

void cal_donut_entry(PtwID* p, double min, Mesh* mesh_p, C_RTree* r_p, bool debug_mode, bool small_set, Entry* prem_entry /*, Entry_Stat& es*/) {

    PtwID a, b, c;

    if (debug_mode) cout << TAB << "Pt #" << p->id << endl;

    // start looking for the first subsidiary pt a
    if (debug_mode) timer_start();

    int nn_a;
    auto nn_d_a = r_p->nn_sphere(p->pt, min, &nn_a);

    if (debug_mode) cout << TABTAB << "First pt #" << nn_a << " dist=" << nn_d_a << " in " << timer_end(MILLISECOND) << " (ms)" << endl;

    // cout << "nn_a: " << nn_a << endl;
    if (nn_a >= 0) {
        a.set(nn_a, mesh_p);
        // es.nn_dist[0] = nn_d_a;
    } else {
        return;
    }

    // start looking for the second pt b
    if (debug_mode) timer_start();

    Pt3D m; middle_pt(p->pt, a.pt, m);
    auto pa = *p->pt - *a.pt;

    double d_pm = eucl_dist(p->pt, &m);

    double r_donut = d_pm * 1.73205080757;

    int nn_b;
    // auto nn_d_b = r_p->donut_nn(&m, r_donut, &pa, &nn_b);
    double nn_d_b;
    if (small_set) {
        nn_d_b = donut_nn_quick(&m, &pa, r_donut, mesh_p, r_p, &nn_b);
    } else {
        nn_d_b = donut_nn(&m, &pa, r_donut, mesh_p, r_p, &nn_b);
    }

    if (debug_mode) cout << TABTAB << "Second pt #" << nn_b << " dist=" << nn_d_b << " in " << timer_end(MILLISECOND) << " (ms)" << endl;

    // cout << "nn_b: " << nn_b << endl;
    if (nn_b >= 0) {
        b.set(nn_b, mesh_p);
        // es.nn_dist[1] = nn_d_b;
    } else {
        return;
    }

    // start looking for the third pt c
    if (debug_mode) timer_start();

    Pt3D b_est, c_est; // but b_est will not be used
    auto got_b_c = get_est_b_c(&m, a.pt, b.pt, b_est, c_est);
    if (!got_b_c) {
        return;
    }

    int nn_c = -1;
    auto nn_d_c = r_p->nn_sphere(&c_est, 0.0, &nn_c, { p->id, a.id, b.id });
    if (nn_c >= 0 && nn_c < mesh_p->size()) {
        c = PtwID(nn_c, mesh_p);
        // es.nn_dist[2] = nn_d_c;
    } else {
        return;
    }

    if (debug_mode) cout << TABTAB << "Third pt #" << nn_c << " dist=" << nn_d_c << " in " << timer_end(MILLISECOND) << " (ms)" << endl;

    // get the ratio set
    auto ratio_set = get_ratio_set_vol(p->pt, a.pt, b.pt, c.pt);

    if (ratio_set.ratio - prem_entry->meas > 0) {
        prem_entry->set(p, &a, &b, &c, ratio_set.volume, ratio_set.ratio);
        prem_entry->fail = false;
    }

}

void cal_index_entry(PtwID* p, double min, Mesh* mesh_p, C_RTree* r_p, bool debug_mode, Entry* prem_entry) {

	PtwID a, b, c, h;

    if (debug_mode) cout << TAB << "Pt #" << p->id << endl;

    // start looking for the first subsidiary pt a
    // if (debug_mode) timer_start();

    int nn_a;
    auto nn_d_a = r_p->nn_sphere(p->pt, min, &nn_a);

    // if (debug_mode) cout << TABTAB << "First pt #" << nn_a << " dist=" << nn_d_a << " in " << timer_end(MILLISECOND) << " (ms)" << endl;

    if (nn_a >= 0) {
     	a.set(nn_a, mesh_p);
    } else {
    	return;
    }

    // start looking for the help pt h
    // if (debug_mode) timer_start();

    Pt3D m; middle_pt(p->pt, a.pt, m);

    double d_pm = eucl_dist(p->pt, &m);
    int nn_h;
    auto nn_d_h = r_p->nn_sphere(&m, d_pm, &nn_h, { p->id, a.id });

    // if (debug_mode) cout << TABTAB << "Help pt #" << nn_h << " dist=" << nn_d_h << " in " << timer_end(MILLISECOND) << " (ms)" << endl;

    if (nn_h >= 0) {
     	h = PtwID(nn_h, mesh_p);
    } else {
    	return;
    }

    // if (debug_mode) { cout << TABTAB; printf("hp=%f, ha=%f\n", eucl_dist(h.pt, p->pt), eucl_dist(h.pt, a.pt)); }

    // start looking for the rest two pts b and c
    // if (debug_mode) timer_start();

    Pt3D b_est, c_est;
    auto got_b_c = get_est_b_c(&m, a.pt, h.pt, b_est, c_est);
    if (!got_b_c) {
    	return;
    }

    int nn_b;
    auto nn_d_b = r_p->nn_sphere(&b_est, 0.0, &nn_b, { p->id, a.id });
    if (nn_b >= 0) {
     	b = PtwID(nn_b, mesh_p);
    } else {
    	return;
    }

    int nn_c;
    auto nn_d_c = r_p->nn_sphere(&c_est, 0.0, &nn_c, { p->id, a.id, b.id });
    if (nn_c >= 0) {
     	c = PtwID(nn_c, mesh_p);
    } else {
    	return;
    }

    // if (debug_mode) cout << TABTAB << "Second pt #" << nn_b << " dist=" << nn_d_b
    // 	<< ", third pt #" << nn_c << " dist=" << nn_d_c << " in " << timer_end(MILLISECOND) << " (ms)" << endl;

    // get the ratio set
    auto ratio_set = get_ratio_set_vol(p->pt, a.pt, b.pt, c.pt);

    if (ratio_set.ratio - prem_entry->meas > 0) {
    	prem_entry->set(p, &a, &b, &c, ratio_set.volume, ratio_set.ratio, &h);
    	prem_entry->fail = false;
    }

}

void analyze_entry(Entry* e, double &avg, double &min, double &max, double &sd) {
    min = numeric_limits<double>::max();
    max = -1;
    avg = 0.0;
    for (int i = 0; i < 6; i++) {
        avg += e->sides[i];
        if (e->sides[i] > max) {
            max = e->sides[i];
        }
        if (e->sides[i] < min) {
            min = e->sides[i];
        }
    }
    avg /= 6.0;
    sd = 0.0;
    for (int i = 0; i < 6; i++) {
        sd += (e->sides[i] - avg) * (e->sides[i] - avg);
    }
    sd /= 6.0;
    sd = sqrt(sd);
}

int main(int argc, char **argv) {

    srand(time(0));

    if (argc < 9) {
        cerr << "Usage: " << argv[0] << " database_path output_grid_filename alg rad ang_thresh thickness is_simp is_small [-show_prog_bar] [-debug] [-num_parts=...]" << endl;
        exit(1);
    }

    bool show_prog_bar = false, debug_mode = false;
    int num_parts = 0;
    for (int i = 0; i < argc; i++) {
        string argv_str(argv[i]);
             if (argv_str.rfind("-show_prog_bar", 0) == 0) show_prog_bar = true;
        else if (argv_str.rfind("-debug", 0) == 0)         debug_mode = true;
        else if (argv_str.rfind("-num_parts", 0) == 0)     num_parts = atoi(argv[i] + 11);
    }

    int argi = 0;
    const string db_path = argv[++argi];
    const string outgrid_filename = argv[++argi];
    const string algorithm = argv[++argi];
    const double w = 0.0;
    const double radius = atof(argv[++argi]);
    const double ang_min = atof(argv[++argi]);
    const double ann_thn = atof(argv[++argi]);
    const int is_simp = atoi(argv[++argi]);
    const int is_small = atoi(argv[++argi]);
    // const int use_color = atoi(argv[++argi]);

    const double cos_phi = cos(ang_min * PI / 180.0);

    const double radius_1 = radius + ann_thn;
    const double radius_2 = radius_1 + ann_thn;

    cout << "Parameters:" << endl;
    cout << "db_path = " << db_path << endl;
    cout << "algorithm = " << algorithm << endl;
    cout << "w = " << w << endl;
    cout << "radius = " << radius << endl;
    cout << "radius_1 = " << radius_1 << endl;
    cout << "radius_2 = " << radius_2 << endl;
    cout << "ang_min = " << ang_min << endl;
    cout << "is_simp = " << is_simp << endl;
    cout << "is_small = " << is_small << endl;
    cout << "num_parts = " << num_parts << endl;
    cout << "outgrid_filename = " << outgrid_filename << endl;
    cout << endl;

    timer_start();

    cout << "Reading database files from " << db_path << endl;
    DB_Meshes db_meshes;
    const int num_meshes = db_meshes.read_from_path(db_path);
    if (num_meshes < 0) {
        cerr << "Error reading database files" << endl;
        exit(1);
    }
    cout << "Total no. meshes: " << num_meshes << endl;

    const int n = db_meshes.total();
    cout << "Total points: " << n << endl << endl;

    cout << "Reading database R-trees..." << endl;
    vector<C_RTree*> db_rtrees;
    if (num_parts > 0) {
        const int num_meshes_per_part = num_meshes / num_parts;
        for (int i = 0; i < num_parts; i++) {
            read_rtrees_comb(db_path, i, num_meshes_per_part, db_rtrees);
        }
    } else {
        read_rtrees_from_db_meshes(&db_meshes, db_rtrees);
    }

    cout << endl;

    const double i_time = timer_end(SECOND);

    cout << "Total no. pts: " << n << endl << endl;
    cout << "Total I/- time: " << i_time << "(s)" << endl;


    timer_start();

    // Open grid file and write grid headers
    ofstream ofs(outgrid_filename);
    ofs << w << " " << radius << " " << radius_1 << " " << radius_2 << " " << ang_min << " " << num_meshes << " " << db_meshes.total() << endl;
    for (int i = 0; i < num_meshes; i++) {
        ofs << i << " 0 0 0 0 0 0" << endl;
    }

    // Open binary file and write binary headers
    ofstream bin_ofs(get_bin_filename(outgrid_filename), ios::out | ios::binary);
    Struct_DB_Helper hdr { w, radius, radius_1, radius_2, ang_min };
    bin_ofs.write((char *) &hdr, sizeof(Struct_DB_Helper));

    // setting up precision for writing
    cout << setiosflags(ios::fixed) << setprecision(2);

    PtwID p;
    Mesh* mesh_p;
    ProgressBar* bar;
    int i;
    Entry* prem_entry;

    Entry_Helper ent;
    auto ptr_ent = (char *) &ent;

    int fail_count = 0;
    int global_cell_id = 0;

    double sum_stat_avg_length = 0.0, sum_stat_max_length = 0.0, sum_stat_min_length = 0.0, sum_stat_sd_length = 0.0;
    double stat_avg_length = 0.0, stat_max_length = 0.0, stat_min_length = 0.0, stat_sd_length = 0.0;
    
    for (int mesh_id = 0; mesh_id < db_meshes.size(); mesh_id++) {
        mesh_p = db_meshes.get_mesh(mesh_id);

        if (mesh_id % 1000 == 0) {
            cout << "Processing mesh #" << mesh_id << endl << endl;
        }

        if (show_prog_bar) {    
            bar = new ProgressBar(mesh_p->size(), 70);
        }

        for (i = 0; i < mesh_p->size(); i++) {
            if (show_prog_bar) {
                ++(*bar);
                bar->display();
            }

            p.set(i, mesh_p);

            prem_entry = new Entry();
            prem_entry->fail = true;

            if (algorithm == "3lnn") {
                cal_3lnn_entry(&p, radius, radius_1, radius_2, cos_phi, mesh_p, db_rtrees[mesh_id], debug_mode, is_simp, prem_entry);
            } else if (algorithm == "3nn") {
                cal_3nn_entry(&p, radius, cos_phi, mesh_p, db_rtrees[mesh_id], debug_mode, is_simp, prem_entry);
            } else if (algorithm == "donut") {
                cal_donut_entry(&p, radius, mesh_p, db_rtrees[mesh_id], debug_mode, is_small, prem_entry);
            } else if (algorithm == "GT") {
                cal_index_entry(&p, radius, mesh_p, db_rtrees[mesh_id], debug_mode, prem_entry);
            }

            if (prem_entry->fail) {
                if (debug_mode) cout << TAB << "Fail in finding prem entry" << endl;
                fail_count++;
            } else {
                if (debug_mode) cout << TAB << "Prem entry: " << prem_entry->to_str() << endl;

                // for edge index
                prem_entry->fill_sides();

                #ifdef _CLR
                    prem_entry->fill_color(mesh_p);
                #endif

                analyze_entry(prem_entry, stat_avg_length, stat_min_length, stat_max_length, stat_sd_length);
                sum_stat_avg_length += stat_avg_length;
                sum_stat_min_length += stat_min_length;
                sum_stat_max_length += stat_max_length;
                sum_stat_sd_length += stat_sd_length;
            }

            ofs << mesh_id << " " << i << " " << global_cell_id << " 0 0 0 1 " << i << endl;
            ofs << mesh_id << " " << prem_entry->to_str(12) << endl;

            prem_entry->get_entry_helper(ent);
            bin_ofs.write(ptr_ent, sizeof(Entry_Helper));

            delete prem_entry;

            global_cell_id++;

            if (debug_mode && i >= 1000) {
                break;
            }

        }

        if (debug_mode) {
            break;
        }

        if (show_prog_bar) {
            bar->done();
            delete bar;
        }

        delete db_rtrees[mesh_id];
        db_rtrees[mesh_id] = nullptr;
    }

    ofs.close();
    bin_ofs.close();

    const double user_time = timer_end(SECOND);

    cout << resetiosflags(ios::fixed) << setprecision(6);

    cout << "Total # of failed cells: " << fail_count << endl << endl;
    cout << "Total user time: " << user_time << "(s)" << endl << endl;

    double valid_num = (double) (n - fail_count);
    double avg_stat_avg_length = sum_stat_avg_length / valid_num;
    double avg_stat_min_length = sum_stat_min_length / valid_num;
    double avg_stat_max_length = sum_stat_max_length / valid_num;
    double avg_stat_sd_length = sum_stat_sd_length / valid_num;


    string outgrid_stat_filename = outgrid_filename + ".gstat";
    ofstream ofs_stat(outgrid_stat_filename);

    ofs_stat << "num_pts=" << n << endl;
    ofs_stat << "num_fail=" << fail_count << endl;
    ofs_stat << "I_time=" << i_time << endl;
    ofs_stat << "user_time=" << user_time << endl;

    ofs_stat << "avg_avg=" << avg_stat_avg_length << endl;
    ofs_stat << "avg_min=" << avg_stat_min_length << endl;
    ofs_stat << "avg_max=" << avg_stat_max_length << endl;
    ofs_stat << "avg_sd=" << avg_stat_sd_length << endl;

    ofs_stat.close();

}