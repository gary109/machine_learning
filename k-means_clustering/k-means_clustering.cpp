// kmeans.c
// Ethan Brodsky
// October 2011

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctime>

using namespace std; 

#define sqr(x) ((x)*(x))

#define MAX_CLUSTERS 16

#define MAX_ITERATIONS 100

#define BIG_double (INFINITY)

void fail(char *str)
  {
    printf(str);
    exit(-1);
  }
  
double calc_distance(int dim, double *p1, double *p2)
  {
    double distance_sq_sum = 0;
    
    for (int ii = 0; ii < dim; ii++)
      distance_sq_sum += sqr(p1[ii] - p2[ii]);

    return distance_sq_sum;
    
  }

void calc_all_distances(int dim, int n, int k, double *X, double *centroid, double *distance_output)
  {
    for (int ii = 0; ii < n; ii++) // for each point
      for (int jj = 0; jj < k; jj++) // for each cluster
        {
         // calculate distance between point and cluster centroid
          distance_output[ii*k + jj] = calc_distance(dim, &X[ii*dim], &centroid[jj*dim]);
        }
  }
  
double calc_total_distance(int dim, int n, int k, double *X, double *centroids, int *cluster_assignment_index)
 // NOTE: a point with cluster assignment -1 is ignored
  {
    double tot_D = 0;
    
   // for every point
    for (int ii = 0; ii < n; ii++)
      {
       // which cluster is it in?
        int active_cluster = cluster_assignment_index[ii];
        
       // sum distance
        if (active_cluster != -1)
          tot_D += calc_distance(dim, &X[ii*dim], &centroids[active_cluster*dim]);
      }
      
    return tot_D;
  }

void choose_all_clusters_from_distances(int dim, int n, int k, double *distance_array, int *cluster_assignment_index)
  {
   // for each point
    for (int ii = 0; ii < n; ii++)
      {
        int best_index = -1;
        double closest_distance = BIG_double;
        
       // for each cluster
        for (int jj = 0; jj < k; jj++)
          {
           // distance between point and cluster centroid
           
            double cur_distance = distance_array[ii*k + jj];
            if (cur_distance < closest_distance)
              {
                best_index = jj;
                closest_distance = cur_distance;
              }
          }

       // record in array
        cluster_assignment_index[ii] = best_index;
      }
  }

void calc_cluster_centroids(int dim, int n, int k, double *X, int *cluster_assignment_index, double *new_cluster_centroid)
  {
    int cluster_member_count[MAX_CLUSTERS];
  
   // initialize cluster centroid coordinate sums to zero
    for (int ii = 0; ii < k; ii++) 
      {
        cluster_member_count[ii] = 0;
        
        for (int jj = 0; jj < dim; jj++)
          new_cluster_centroid[ii*dim + jj] = 0;
     }

   // sum all points
   // for every point
    for (int ii = 0; ii < n; ii++)
      {
       // which cluster is it in?
        int active_cluster = cluster_assignment_index[ii];

       // update count of members in that cluster
        cluster_member_count[active_cluster]++;
        
       // sum point coordinates for finding centroid
        for (int jj = 0; jj < dim; jj++)
          new_cluster_centroid[active_cluster*dim + jj] += X[ii*dim + jj];
      }
      
      
   // now divide each coordinate sum by number of members to find mean/centroid
   // for each cluster
    for (int ii = 0; ii < k; ii++) 
      {
        if (cluster_member_count[ii] == 0)
          printf("WARNING: Empty cluster %d! \n", ii);
          
       // for each dimension
        for (int jj = 0; jj < dim; jj++)
          new_cluster_centroid[ii*dim + jj] /= cluster_member_count[ii];  /// XXXX will divide by zero here for any empty clusters!

      }
  }

void get_cluster_member_count(int n, int k, int *cluster_assignment_index, int *cluster_member_count)
  {
   // initialize cluster member counts
    for (int ii = 0; ii < k; ii++) 
      cluster_member_count[ii] = 0;
  
   // count members of each cluster    
    for (int ii = 0; ii < n; ii++)
      cluster_member_count[cluster_assignment_index[ii]]++;
  }

void update_delta_score_table(int dim, int n, int k, double *X, int *cluster_assignment_cur, double *cluster_centroid, int *cluster_member_count, double *point_move_score_table, int cc)
  {
   // for every point (both in and not in the cluster)
    for (int ii = 0; ii < n; ii++)
      {
        double dist_sum = 0;
        for (int kk = 0; kk < dim; kk++)
          {
            double axis_dist = X[ii*dim + kk] - cluster_centroid[cc*dim + kk]; 
            dist_sum += sqr(axis_dist);
          }
          
        double mult = ((double)cluster_member_count[cc] / (cluster_member_count[cc] + ((cluster_assignment_cur[ii]==cc) ? -1 : +1)));

        point_move_score_table[ii*dim + cc] = dist_sum * mult;
      }
  }
  
  
void  perform_move(int dim, int n, int k, double *X, int *cluster_assignment, double *cluster_centroid, int *cluster_member_count, int move_point, int move_target_cluster)
  {
    int cluster_old = cluster_assignment[move_point];
    int cluster_new = move_target_cluster;
  
   // update cluster assignment array
    cluster_assignment[move_point] = cluster_new;
    
   // update cluster count array
    cluster_member_count[cluster_old]--;
    cluster_member_count[cluster_new]++;
    
    if (cluster_member_count[cluster_old] <= 1)
      printf("WARNING: Can't handle single-member clusters! \n");
    
   // update centroid array
    for (int ii = 0; ii < dim; ii++)
      {
        cluster_centroid[cluster_old*dim + ii] -= (X[move_point*dim + ii] - cluster_centroid[cluster_old*dim + ii]) / cluster_member_count[cluster_old];
        cluster_centroid[cluster_new*dim + ii] += (X[move_point*dim + ii] - cluster_centroid[cluster_new*dim + ii]) / cluster_member_count[cluster_new];
      }
  }  
  
void cluster_diag(int dim, int n, int k, double *X, int *cluster_assignment_index, double *cluster_centroid)
  {
    int cluster_member_count[MAX_CLUSTERS];
    
    get_cluster_member_count(n, k, cluster_assignment_index, cluster_member_count);
     
    printf("  Final clusters \n");
    for (int ii = 0; ii < k; ii++) 
      printf("    cluster %d:     members: %8d, centroid (%.1f %.1f) \n", ii, cluster_member_count[ii], cluster_centroid[ii*dim + 0], cluster_centroid[ii*dim + 1]);
  }

void copy_assignment_array(int n, int *src, int *tgt)
  {
    for (int ii = 0; ii < n; ii++)
      tgt[ii] = src[ii];
  }
  
int assignment_change_count(int n, int a[], int b[])
  {
    int change_count = 0;

    for (int ii = 0; ii < n; ii++)
      if (a[ii] != b[ii])
        change_count++;
        
    return change_count;
  }

void kmeans(
            int  dim,		                     // dimension of data 

            double *X,                        // pointer to data
            int   n,                         // number of elements
            
            int   k,                         // number of clusters
            double *cluster_centroid,         // initial cluster centroids
            int   *cluster_assignment_final  // output
           )
  {
    double *dist                    = (double *)malloc(sizeof(double) * n * k);
    int   *cluster_assignment_cur  = (int *)malloc(sizeof(int) * n);
    int   *cluster_assignment_prev = (int *)malloc(sizeof(int) * n);
    double *point_move_score        = (double *)malloc(sizeof(double) * n * k);
    
    
    if (!dist || !cluster_assignment_cur || !cluster_assignment_prev || !point_move_score)
      fail("Error allocating dist arrays");
    
   // initial setup  
    calc_all_distances(dim, n, k, X, cluster_centroid, dist);
    choose_all_clusters_from_distances(dim, n, k, dist, cluster_assignment_cur);
    copy_assignment_array(n, cluster_assignment_cur, cluster_assignment_prev);

   // BATCH UPDATE
    double prev_totD = BIG_double;
    int batch_iteration = 0;
    while (batch_iteration < MAX_ITERATIONS)
      {
//        printf("batch iteration %d \n", batch_iteration);
//        cluster_diag(dim, n, k, X, cluster_assignment_cur, cluster_centroid);
        
        // update cluster centroids
         calc_cluster_centroids(dim, n, k, X, cluster_assignment_cur, cluster_centroid);

        // deal with empty clusters
        // XXXXXXXXXXXXXX

        // see if we've failed to improve
         double totD = calc_total_distance(dim, n, k, X, cluster_centroid, cluster_assignment_cur);
         if (totD > prev_totD)
          // failed to improve - currently solution worse than previous
           {
            // restore old assignments
             copy_assignment_array(n, cluster_assignment_prev, cluster_assignment_cur);
             
            // recalc centroids
             calc_cluster_centroids(dim, n, k, X, cluster_assignment_cur, cluster_centroid);
             
             printf("  negative progress made on this step - iteration completed (%.2f) \n", totD - prev_totD);
             
            // done with this phase
             break;
           }
           
        // save previous step
         copy_assignment_array(n, cluster_assignment_cur, cluster_assignment_prev);
         
        // move all points to nearest cluster
         calc_all_distances(dim, n, k, X, cluster_centroid, dist);
         choose_all_clusters_from_distances(dim, n, k, dist, cluster_assignment_cur);
         
         int change_count = assignment_change_count(n, cluster_assignment_cur, cluster_assignment_prev);
         
         printf("%3d   %u   %9d  %16.2f %17.2f\n", batch_iteration, 1, change_count, totD, totD - prev_totD);
         fflush(stdout);
         
        // done with this phase if nothing has changed
         if (change_count == 0)
           {
             printf("  no change made on this step - iteration completed \n");
             break;
           }

         prev_totD = totD;
                        
         batch_iteration++;
      }

cluster_diag(dim, n, k, X, cluster_assignment_cur, cluster_centroid);


/* THe online update prtion of this code has never worked properly, but batch update has been adequate for our projects so far
   // ONLINE UPDATE
    int online_iteration = 0;
    int last_point_moved = 0;
    
    int cluster_changed[MAX_CLUSTERS];
    for (int ii = 0; ii < k; ii++)
      cluster_changed[ii] = 1;
    
    int cluster_member_count[MAX_CLUSTERS];
    get_cluster_member_count(n, k, cluster_assignment_cur, cluster_member_count);
    
    while (online_iteration < MAX_ITERATIONS)
      {
//        printf("online iteration %d \n", online_iteration);

       // for each cluster
        for (int ii = 0; ii < k; ii++)
          if (cluster_changed[ii])
            update_delta_score_table(dim, n, k, X, cluster_assignment_cur, cluster_centroid, cluster_member_count, point_move_score, ii);
            
       // pick a point to move
       // look at points in sequence starting at one after previously moved point
        int make_move = 0;
        int point_to_move = -1;
        int target_cluster = -1;
        for (int ii = 0; ii < n; ii++)
          {
            int point_to_consider = (last_point_moved + 1 + ii) % n;
              
           // find the best target for it
            int best_target_cluster = -1;
            int best_match_count    = 0;
            double best_delta        = BIG_double;
            
           // for each possible target
            for (int jj = 0; jj < k; jj++)
              {
                double cur_delta = point_move_score[point_to_consider*k + jj];

               // is this the best move so far?
                if (cur_delta < best_delta)
                 // yes - record it
                  {
                    best_target_cluster = jj;
                    best_delta = cur_delta;
                    best_match_count = 1;
                  }
                else if (cur_delta == best_delta)
                 // no, but it's tied with the best one
                 best_match_count++;
              }

           // is the best cluster for this point its current cluster?
            if (best_target_cluster == cluster_assignment_cur[point_to_consider])
             // yes - don't move this point
               continue;

           // do we have a unique best move?
            if (best_match_count > 1)
             // no - don't move this point (ignore ties)
              continue;
            else
             // yes - we've found a good point to move
              {
                point_to_move = point_to_consider;
                target_cluster = best_target_cluster;
                make_move = 1;
                break;
              }
          }

        if (make_move)
          {
           // where should we move it to?            
            printf("  %10d: moved %d to %d \n", point_to_move, cluster_assignment_cur[point_to_move], target_cluster);

           // mark which clusters have been modified          
            for (int ii = 0; ii < k; ii++)
              cluster_changed[ii] = 0;
            cluster_changed[cluster_assignment_cur[point_to_move]] = 1;
            cluster_changed[target_cluster] = 1;

           // perform move
            perform_move(dim, n, k, X, cluster_assignment_cur, cluster_centroid, cluster_member_count, point_to_move, target_cluster);

           // count an iteration every time we've cycled through all the points
            if (point_to_move < last_point_moved)
              online_iteration++;

            last_point_moved = point_to_move;
          }

      }

*/
      
//    printf("iterations: %3d %3d \n", batch_iteration, online_iteration);
      
   // write to output array
    copy_assignment_array(n, cluster_assignment_cur, cluster_assignment_final);    
    
    free(dist);
    free(cluster_assignment_cur);
    free(cluster_assignment_prev);
    free(point_move_score);
  }           
          #define NUM_YEARS     280 
double                  pattern [NUM_YEARS] = {

0.0262,  0.0575,  0.0837,  0.1203,  0.1883,  0.3033,  
0.1517,  0.1046,  0.0523,  0.0418,  0.0157,  0.0000,  
0.0000,  0.0105,  0.0575,  0.1412,  0.2458,  0.3295,  
0.3138,  0.2040,  0.1464,  0.1360,  0.1151,  0.0575,  
0.1098,  0.2092,  0.4079,  0.6381,  0.5387,  0.3818,  
0.2458,  0.1831,  0.0575,  0.0262,  0.0837,  0.1778,  
0.3661,  0.4236,  0.5805,  0.5282,  0.3818,  0.2092,  
0.1046,  0.0837,  0.0262,  0.0575,  0.1151,  0.2092,  
0.3138,  0.4231,  0.4362,  0.2495,  0.2500,  0.1606,  
0.0638,  0.0502,  0.0534,  0.1700,  0.2489,  0.2824,  
0.3290,  0.4493,  0.3201,  0.2359,  0.1904,  0.1093,  
0.0596,  0.1977,  0.3651,  0.5549,  0.5272,  0.4268,  
0.3478,  0.1820,  0.1600,  0.0366,  0.1036,  0.4838,  
0.8075,  0.6585,  0.4435,  0.3562,  0.2014,  0.1192,  
0.0534,  0.1260,  0.4336,  0.6904,  0.6846,  0.6177,  
0.4702,  0.3483,  0.3138,  0.2453,  0.2144,  0.1114,  
0.0837,  0.0335,  0.0214,  0.0356,  0.0758,  0.1778,  
0.2354,  0.2254,  0.2484,  0.2207,  0.1470,  0.0528,  
0.0424,  0.0131,  0.0000,  0.0073,  0.0262,  0.0638,  
0.0727,  0.1851,  0.2395,  0.2150,  0.1574,  0.1250,  
0.0816,  0.0345,  0.0209,  0.0094,  0.0445,  0.0868,  
0.1898,  0.2594,  0.3358,  0.3504,  0.3708,  0.2500,  
0.1438,  0.0445,  0.0690,  0.2976,  0.6354,  0.7233,  
0.5397,  0.4482,  0.3379,  0.1919,  0.1266,  0.0560,  
0.0785,  0.2097,  0.3216,  0.5152,  0.6522,  0.5036,  
0.3483,  0.3373,  0.2829,  0.2040,  0.1077,  0.0350,  
0.0225,  0.1187,  0.2866,  0.4906,  0.5010,  0.4038,  
0.3091,  0.2301,  0.2458,  0.1595,  0.0853,  0.0382,  
0.1966,  0.3870,  0.7270,  0.5816,  0.5314,  0.3462,  
0.2338,  0.0889,  0.0591,  0.0649,  0.0178,  0.0314,  
0.1689,  0.2840,  0.3122,  0.3332,  0.3321,  0.2730,  
0.1328,  0.0685,  0.0356,  0.0330,  0.0371,  0.1862,  
0.3818,  0.4451,  0.4079,  0.3347,  0.2186,  0.1370,  
0.1396,  0.0633,  0.0497,  0.0141,  0.0262,  0.1276,  
0.2197,  0.3321,  0.2814,  0.3243,  0.2537,  0.2296,  
0.0973,  0.0298,  0.0188,  0.0073,  0.0502,  0.2479,  
0.2986,  0.5434,  0.4215,  0.3326,  0.1966,  0.1365,  
0.0743,  0.0303,  0.0873,  0.2317,  0.3342,  0.3609,  
0.4069,  0.3394,  0.1867,  0.1109,  0.0581,  0.0298,  
0.0455,  0.1888,  0.4168,  0.5983,  0.5732,  0.4644,  
0.3546,  0.2484,  0.1600,  0.0853,  0.0502,  0.1736,  
0.4843,  0.7929,  0.7128,  0.7045,  0.4388,  0.3630,  
0.1647,  0.0727,  0.0230,  0.1987,  0.7411,  0.9947,  
0.9665,  0.8316,  0.5873,  0.2819,  0.1961,  0.1459,  
0.0534,  0.0790,  0.2458,  0.4906,  0.5539,  0.5518,  
0.5465,  0.3483,  0.3603,  0.1987,  0.1804,  0.0811,  
0.0659,  0.1428,  0.4838,  0.8127 

};
int main(){


srand ( unsigned ( time(0) ) );


  double cluster_centroid = rand()%NUM_YEARS;
  int  cluster_assignment_final;

  kmeans(0,	pattern, 20, 5, &cluster_centroid, &cluster_assignment_final);

  return 0;
}