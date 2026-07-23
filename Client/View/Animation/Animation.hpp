#pragma once

#include <vector>
#include <memory>
#include <stdexcept>
#include <algorithm>
#include "AnimationConfig.hpp"
#include "img.hpp"
/*
 * @class Animation
 * @brief Manages a sequence of frames that constitute a piece's animation.
 * Purpose: Handles loading, storing, and selecting frames to display based on elapsed time.
 * Responsibility: Frame management, setting FPS, loop support, and frame retrieval by time.
 * Created by: ResourceManager | Used by: PieceRenderer.
 * Functions: addFrame(), set/getFramesPerSecond(), set/isLoop(), empty/frameCount(), getFrameByTime/List().
 */
class Animation
{
private:
    std::vector<std::shared_ptr<Img>> frames;

    int framesPerSecond =
        AnimationConfig::DEFAULT_FRAMES_PER_SECOND;

    bool loop =
        AnimationConfig::DEFAULT_LOOP;

public:
    Animation() = default;

    void addFrame(std::shared_ptr<Img> frame)
    {
        frames.push_back(frame);
    }

    void setFramesPerSecond(int fps)
    {
        framesPerSecond = fps;
    }

    int getFramesPerSecond() const
    {
        return framesPerSecond;
    }

    void setLoop(bool value)
    {
        loop = value;
    }

    bool isLoop() const
    {
        return loop;
    }

    bool empty() const
    {
        return frames.empty();
    }

    int frameCount() const
    {
        return static_cast<int>(frames.size());
    }

    std::shared_ptr<Img> getFrame(int index)
    {
        if (frames.empty())
        {
            throw std::runtime_error("Animation contains no frames.");
        }

        if (index < 0)
        {
            index = 0;
        }

        index %= frames.size();

        return frames[index];
    }

    std::shared_ptr<Img> getFrameByTime(int elapsedMs)
    {
        if (frames.empty())
        {
            throw std::runtime_error("Animation contains no frames.");
        }

        int frame =
            (elapsedMs * framesPerSecond) / 1000;

        if (loop)
        {
            frame %= frames.size();
        }
        else
        {
            frame = std::min(
                frame,
                (int)frames.size() - 1);
        }

        return frames[frame];
    }

    const std::vector<std::shared_ptr<Img>> &getFrames() const
    {
        return frames;
    }
};