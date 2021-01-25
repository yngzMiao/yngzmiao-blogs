import numpy as np
# belief = np.array([1/10]*10)
# print(belief)

# hallway = np.array([1, 1, 0, 0, 0, 0, 0, 0, 1, 0])

import numpy as np
import matplotlib.pyplot as plt

def bar_plot(pos, x=None, ylim=(0,1), title=None, c='#30a2da',
        **kwargs):
  ax = plt.gca()
  if x is None:
    x = np.arange(len(pos))
  ax.bar(x, pos, color=c, **kwargs)
  if ylim:
    plt.ylim(ylim)
  plt.xticks(np.asarray(x), x)
  if title is not None:
    plt.title(title)
  plt.show()

# belief = np.array([1 / 3, 1 / 3, 0, 0, 0, 0, 0, 0, 1 / 3, 0])
# bar_plot(belief)

# belief = hallway * (1/3)
# print(belief)

# belief = np.array([0., 1., 0., 0., 0., 0., 0., 0., 0., 0.])

def update_belief(hall, belief, z, correct_scale):
  for i, val in enumerate(hall):
    if val == z:
      belief[i] *= correct_scale

# hallway = np.array([1, 1, 0, 0, 0, 0, 0, 0, 1, 0])
# belief = np.array([0.1] * 10)
# reading = 1                 # 1 is 'door'
# update_belief(hallway, belief, z=reading, correct_scale=3.)
# print('belief:', belief)
# print('sum =', sum(belief))
# plt.figure()
# bar_plot(belief)

# belief / sum(belief)

from filterpy.discrete_bayes import normalize
# normalize(belief)

from filterpy.discrete_bayes import normalize

def scaled_update(hall, belief, z, z_prob):
  scale = z_prob / (1. - z_prob)
  belief[hall==z] *= scale
  normalize(belief)

# hallway = np.array([1, 1, 0, 0, 0, 0, 0, 0, 1, 0])
# belief = np.array([0.1] * 10)
# scaled_update(hallway, belief, z=1, z_prob=.75)

# print('sum =', sum(belief))
# print('probability of door =', belief[0])
# print('probability of wall =', belief[2])
# bar_plot(belief, ylim=(0, .3))

def scaled_update(hall, belief, z, z_prob): 
  scale = z_prob / (1. - z_prob)
  likelihood = np.ones(len(hall))
  likelihood[hall==z] *= scale
  return normalize(likelihood * belief)

def update(likelihood, prior):
  return normalize(likelihood * prior)

from filterpy.discrete_bayes import update

def lh_hallway(hall, z, z_prob):
  try:
    scale = z_prob / (1. - z_prob)
  except ZeroDivisionError:
    scale = 1e8
  likelihood = np.ones(len(hall))
  likelihood[hall == z] *= scale
  return likelihood

# hallway = np.array([1, 1, 0, 0, 0, 0, 0, 0, 1, 0])
# belief = np.array([0.1] * 10)
# likelihood = lh_hallway(hallway, z=1, z_prob=.75)
# update(likelihood, belief)

def perfect_predict(belief, move):
    n = len(belief)
    result = np.zeros(n)
    for i in range(n):
        result[i] = belief[(i - move) % n]
    return result

# belief = np.array([.35, .1, .2, .3, 0, 0, 0, 0, 0, .05])
# plt.subplot(121)
# bar_plot(belief, title='Before prediction', ylim=(0, .4))

# belief = perfect_predict(belief, 1)
# plt.subplot(122)
# bar_plot(belief, title='After prediction', ylim=(0, .4))

from ipywidgets import interact, IntSlider

# belief = np.array([.35, .1, .2, .3, 0, 0, 0, 0, 0, .05])
# perfect_beliefs = []

for _ in range(20):
  belief = perfect_predict(belief, 1)
  perfect_beliefs.append(belief)

def simulate(time_step):
  bar_plot(perfect_beliefs[time_step], ylim=(0, .4))

# interact(simulate, time_step=IntSlider(value=0, max=len(perfect_beliefs) - 1))

def plot_belief_vs_prior(belief, prior, **kwargs):
  plt.subplot(121)
  bar_plot(belief, title='belief', **kwargs)
  plt.subplot(122)
  bar_plot(prior, title='prior', **kwargs)
  plt.show()

def predict_move(belief, move, p_under, p_correct, p_over):
  n = len(belief)
  prior = np.zeros(n)
  for i in range(n):
    prior[i] = (
      belief[(i-move) % n]   * p_correct +
      belief[(i-move-1) % n] * p_over +
      belief[(i-move+1) % n] * p_under)
  return prior

# belief = [0., 0., 0., 1., 0., 0., 0., 0., 0., 0.]
# prior = predict_move(belief, 2, .1, .8, .1)
# plot_belief_vs_prior(belief, prior)

# belief = [0, 0, .4, .6, 0, 0, 0, 0, 0, 0]
# prior = predict_move(belief, 2, .1, .8, .1)
# plot_belief_vs_prior(belief, prior)

# belief = np.array([1.0, 0, 0, 0, 0, 0, 0, 0, 0, 0])
# predict_beliefs = []

for i in range(100):
  belief = predict_move(belief, 1, .1, .8, .1)
  predict_beliefs.append(belief)

# print('Final Belief:', belief)

def show_prior(step):
  bar_plot(predict_beliefs[step-1])
  plt.title('Step {}'.format(step))

# interact(show_prior, step=IntSlider(value=1, max=len(predict_beliefs)))

def predict_move_convolution(pdf, offset, kernel):
  N = len(pdf)
  kN = len(kernel)
  width = int((kN - 1) / 2)

  prior = np.zeros(N)
  for i in range(N):
    for k in range(kN):
      index = (i + (width-k) - offset) % N
      prior[i] += pdf[index] * kernel[k]
  return prior

from scipy.ndimage.filters import convolve

# convolve(np.roll(pdf, offset), kernel, mode='wrap')

from filterpy.discrete_bayes import predict

# belief = [.05, .05, .05, .05, .55, .05, .05, .05, .05, .05]
# prior = predict(belief, offset=1, kernel=[.1, .8, .1])
# plot_belief_vs_prior(belief, prior, ylim=(0,0.6))

# prior = predict(belief, offset=3, kernel=[.05, .05, .6, .2, .1])
# plot_belief_vs_prior(belief, prior, ylim=(0,0.6))

def plot_prior_vs_posterior(prior, posterior, reverse=False, **kwargs):
  if reverse:
    plt.subplot(121)
    bar_plot(posterior, title='posterior', **kwargs)
    plt.subplot(122)
    bar_plot(prior, title='prior', **kwargs)
  else:
    plt.subplot(121)
    bar_plot(prior, title='prior', **kwargs)
    plt.subplot(122)
    bar_plot(posterior, title='posterior', **kwargs)

# hallway = np.array([1, 1, 0, 0, 0, 0, 0, 0, 1, 0])
# prior = np.array([.1] * 10)
# likelihood = lh_hallway(hallway, z=1, z_prob=.75)
# posterior = update(likelihood, prior)
# plot_prior_vs_posterior(prior, posterior, ylim=(0,.5))

# kernel = (.1, .8, .1)
# prior = predict(posterior, 1, kernel)
# plot_prior_vs_posterior(prior, posterior, True, ylim=(0,.5))

# likelihood = lh_hallway(hallway, z=1, z_prob=.75)
# posterior = update(likelihood, prior)
# plot_prior_vs_posterior(prior, posterior, ylim=(0,.5))

# likelihood = lh_hallway(hallway, z=0, z_prob=.75)
# posterior = update(likelihood, prior)
# plot_prior_vs_posterior(prior, posterior, ylim=(0,.5))

# prior = predict(posterior, 1, kernel)
# likelihood = lh_hallway(hallway, z=0, z_prob=.75)
# posterior = update(likelihood, prior)
# plot_prior_vs_posterior(prior, posterior, ylim=(0,.5))

def discrete_bayes_sim(prior, kernel, measurements, z_prob, hallway):
  posterior = np.array([.1] * 10)
  priors, posteriors = [], []
  for i, z in enumerate(measurements):
    prior = predict(posterior, 1, kernel)
    priors.append(prior)

    likelihood = lh_hallway(hallway, z, z_prob)
    posterior = update(likelihood, prior)
    posteriors.append(posterior)
  return priors, posteriors

def plot_posterior(hallway, posteriors, i):
  plt.title('Posterior')
  bar_plot(hallway, c='k')
  bar_plot(posteriors[i], ylim=(0, 1.0))
  plt.axvline(i % len(hallway), lw=5)

def plot_prior(hallway, priors, i):
  plt.title('Prior')
  bar_plot(hallway, c='k')
  bar_plot(priors[i], ylim=(0, 1.0), c='#ff8015')
  plt.axvline(i % len(hallway), lw=5)

def animate_discrete_bayes(hallway, priors, posteriors):
  def animate(step):
    step -= 1
    i = step // 2
    if step % 2 == 0:
      plot_prior(hallway, priors, i)
    else:
      plot_posterior(hallway, posteriors, i)
  return animate

# kernel = (.1, .8, .1)
# z_prob = 1.0
# hallway = np.array([1, 1, 0, 0, 0, 0, 0, 0, 1, 0])
# prior = np.array([.1] * 10)

# measurements with no noise
# zs = [hallway[i % len(hallway)] for i in range(50)]

# priors, posteriors = discrete_bayes_sim(prior, kernel, zs, z_prob, hallway)
# interact(animate_discrete_bayes(hallway, priors, posteriors), step=IntSlider(value=1, max=len(zs)*2))

# kernel = (.1, .8, .1)
# z_prob = 0.75
# hallway = np.array([1, 0, 1, 0, 0] * 2)
# prior = np.array([.1] * 10)

# zs = [1, 0, 1, 0, 0, 1]

# priors, posteriors = discrete_bayes_sim(prior, kernel, zs, z_prob, hallway)
# interact(animate_discrete_bayes(hallway, priors, posteriors), step=IntSlider(value=12, max=len(zs)*2))

# measurements = [1, 0, 1, 0, 0, 1, 1]
# priors, posteriors = discrete_bayes_sim(prior, kernel, measurements, z_prob, hallway)
# plot_posterior(hallway, posteriors, 6)

from contextlib import contextmanager
import matplotlib.pylab as pylab
import matplotlib as mpl

_default_size = (9, 4)

def set_figsize(x=_default_size[0], y=_default_size[1]):
    """ set the figure size of the plot to the specified size in inches"""
    mpl.rcParams['figure.figsize'] = x, y

@contextmanager
def figsize(x=8, y=3):
    """Temporarily set the figure size using 'with figsize(a, b):'"""
    size = pylab.rcParams['figure.figsize']
    set_figsize(x, y)
    yield
    pylab.rcParams['figure.figsize'] = size

# kernel = (.1, .8, .1)
# z_prob = 0.75
# hallway = np.array([1, 0, 1, 0, 0] * 2)
# prior = np.array([.1] * 10)

with figsize(y=5.5):
    measurements = [1, 0, 1, 0, 0, 1, 1, 1, 0, 0]
    for i, m in enumerate(measurements):
        likelihood = lh_hallway(hallway, z=m, z_prob=.75)
        posterior = update(likelihood, prior)
        prior = predict(posterior, 1, kernel)
        plt.subplot(5, 2, i+1)
        bar_plot(posterior, ylim=(0, .4), title='step {}'.format(i+1))
    plt.tight_layout()

class Train(object):
  def __init__(self, track_len, kernel=[1.], sensor_accuracy=.9):
    self.track_len = track_len
    self.pos = 0
    self.kernel = kernel
    self.sensor_accuracy = sensor_accuracy

  def move(self, distance=1):
    """ move in the specified direction
    with some small chance of error"""
    self.pos += distance
    # insert random movement error according to kernel
    r = random.random()
    s = 0
    offset = -(len(self.kernel) - 1) / 2
    for k in self.kernel:
      s += k
      if r <= s:
        break
      offset += 1
    self.pos = int((self.pos + offset) % self.track_len)
    return self.pos

  def sense(self):
    pos = self.pos
    # insert random sensor error
    if random.random() > self.sensor_accuracy:
      if random.random() > 0.5:
        pos += 1
      else:
        pos -= 1
    return pos

def train_filter(iterations, kernel, sensor_accuracy,
      move_distance, do_print=True):
  track = np.array([0, 1, 2, 3, 4, 5, 6, 7, 8, 9])
  prior = np.array([.9] + [0.01] * 9)
  posterior = prior[:]
  normalize(prior)

  robot = Train(len(track), kernel, sensor_accuracy)
  for i in range(iterations):
    # move the robot and
    robot.move(distance=move_distance)

    # peform prediction
    prior = predict(posterior, move_distance, kernel)

    #  and update the filter
    m = robot.sense()
    likelihood = lh_hallway(track, m, sensor_accuracy)
    posterior = update(likelihood, prior)
    index = np.argmax(posterior)

    if do_print:
      print('''time {}: pos {}, sensed {}, '''
          '''at position {}'''.format(
        i, robot.pos, m, track[robot.pos]))

      print('''        estimated position is {}'''
          ''' with confidence {:.4f}%:'''.format(
        index, posterior[index] * 100))

  bar_plot(posterior)
  if do_print:
    print()
    print('final position is', robot.pos)
    index = np.argmax(posterior)
    print('''Estimated position is {} with '''
        '''confidence {:.4f}%:'''.format(
      index, posterior[index] * 100))

import random

# random.seed(3)
# np.set_printoptions(precision=2, suppress=True, linewidth=60)
# train_filter(4, kernel=[1.], sensor_accuracy=.999,
#       move_distance=4, do_print=True)

# random.seed(5)
# train_filter(4, kernel=[.1, .8, .1], sensor_accuracy=.9,
#       move_distance=4, do_print=True)

with figsize(y=5.5):
  for i in range (4):
    random.seed(3)
    plt.subplot(221+i)
    train_filter(148+i, kernel=[.1, .8, .1],
          sensor_accuracy=.8,
          move_distance=4, do_print=False)
    plt.title ('iteration {}'.format(148+i))
