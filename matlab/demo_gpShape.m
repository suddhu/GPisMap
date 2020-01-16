%
% GPisMap - Online Continuous Mapping using Gaussian Process Implicit Surfaces
% https://github.com/leebhoram/GPisMap
%
% This program is free software; you can redistribute it and/or modify
% it under the terms of the GNU General Public License v3 as published by
% the Free Software Foundation.
%
% This program is distributed in the hope that it will be useful, but WITHOUT
% ANY WARRANTY; without even the implied warranty of any FITNESS FOR A
% PARTICULAR PURPOSE. See the GNU General Public License v3 for more details.
%
% You should have received a copy of the GNU General Public License v3
% along with this program; if not, you can access it online at
% http://www.gnu.org/licenses/gpl-3.0.html.
%
% Authors: Bhoram Lee <bhoram.lee@gmail.com>
%

clearvars
close all

addpath(genpath('./util'));
addpath('./plot_scripts');
addpath('../mex');

shape = 'butter';
id = '20200115-2213';

file = strcat('/home/suddhu/software/GPIS/data/contacts/contacts-', shape,'-', id, '.txt');
data = importdata(file);

data(:,3:4) = normr(data(:,3:4)); % unit normals

% test points (regular samples for visualization)
% map limits
xmin = -2.5;
xmax = 2.5;
ymin = -2.5;
ymax = 2.5;
test_intv = 0.1;
[xg, yg] = meshgrid((xmin+test_intv):test_intv:(xmax-test_intv), (ymin+test_intv):test_intv:(ymax-test_intv));
xtest = single([xg(:)'; yg(:)']);

%skip every skip frames
for i=1:size(data,1)

    X_gp = data(i,1:2); % input
    Y_gp = [0, data(i,3:4)]'; % output
        
    % update
    tic
    mexGPShape('update',single(X_gp), single(Y_gp));
    toc
    
    % test visualization
    visualize_gpisMap
%     saveas(gcf, strcat(num2str(nframe),'.png'))

    % % pause if needed
%     disp('Press a button to continue')
%     pause

end

% delete all resources
mexGPShape('reset');
